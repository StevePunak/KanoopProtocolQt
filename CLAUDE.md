# CLAUDE.md — KanoopProtocolQt

Guidance for Claude Code sessions working with this library. Read this before touching code in this submodule.

## Overview

KanoopProtocolQt provides protocol-layer building blocks for Qt 6 applications: an asynchronous HTTP client family (`HttpGet`, `HttpPost`, `HttpPut`, `HttpDelete`, `HttpUpload`, all rooted at `HttpOperation`), and an MQTT client wrapper (`MqttClient`, `MqttParameters`). It does **not** wrap WebSockets, gRPC, or low-level sockets — those live elsewhere.

- Trunk branch: **`master`** (not `main`).
- Public include path: `<Kanoop/http/...>`, `<Kanoop/mqtt/...>` — for example `#include <Kanoop/http/httpget.h>`.
- Export macro: **`LIBKANOOPPROTOCOL_EXPORT`** (defined in `<Kanoop/kanoopprotocol.h>`). Every public class must be marked with it — Windows MinGW link fails otherwise.
- Depends on KanoopCommonQt (uses `AbstractThreadClass`, `TimeSpan`, `Log`, `CommonException`).

## Library conventions

- **Each HTTP operation runs on its own worker thread.** `HttpOperation` subclasses `AbstractThreadClass`. `start()` spins the worker, `execute()` runs there, `waitForCompletion(TimeSpan)` blocks the caller until done or timeout. The QNetworkAccessManager and QNetworkReply are created and destroyed on the worker — never touch them from the caller's thread.
- **Both synchronous and asynchronous patterns are fully supported.** Sync: construct → configure → `start()` → `waitForCompletion(TimeSpan)` → read `success()`/`statusCode()`/`responseBody()` → `delete`. Async: construct → configure → `connect(op, &HttpOperation::operationComplete, ...)` → `start()` → handle in the slot. Pick the one that fits the call site — sync when the caller can block, async when it can't.
- **Errors come through several accessors.** `success()` is the overall pass/fail. `networkError()` is the `QNetworkReply::NetworkError`. `statusCode()` is the HTTP status integer (0 if no response). `reasonPhrase()` is the HTTP reason string. `completionMessage()` is the human-readable summary.
- **No built-in retry.** The library issues exactly one request per operation. Retry is a caller responsibility — loop, construct a new op, `start()` again.
- **No client-certificate support in the base.** `setVerifyPeer(false)` and `ignoreSelfSignedCertificate()` are the only TLS knobs out of the box. Mutual-TLS (client cert + key) requires subclassing and overriding `configureSsl(QNetworkRequest*)`. `MqttClient` (via `MqttParameters`) does support client certs directly.

## Code style

- **Class names**: PascalCase. **Methods**: camelCase. **Members**: underscore-prefixed camelCase (`_foo`). **Statics**: PascalCase, no prefix.
- Opening brace on same line for `if`/`for`/`while`/`try`; on new line for function/method definitions.
- No space before parens in control flow: `if(cond) {`, not `if (cond) {`.
- Explicit boolean comparison: `if(isReady() == false)` and `if(ok == true)`, not `if(!isReady())`.
- Includes ordered: same-library (`"..."`), then dependency libs (`<Kanoop/...>`), then Qt/system.
- File-local helper **functions** used by one class → `static` private members. File-local **constants** → anonymous namespace is fine.
- Doxygen: single-line `/** @brief ... */`; multi-line with `@brief` on the line after `/**`. **Never** document member variables.
- Single-exit functions preferred; avoid early returns.

## Commonly used classes

### `HttpOperation` — `<Kanoop/http/httpoperation.h>` (the base)

Abstract base for every HTTP method class. Owns the worker thread, the network reply, headers, cookies, SSL state, timing. **You don't subclass it for normal use** — instantiate one of the concrete method classes below.

The full surface every concrete subclass inherits:
```cpp
// configuration (call before start())
void appendHeader(QNetworkRequest::KnownHeaders type, const QByteArray& value);
void appendHeader(const QString& headerName,           const QByteArray& value);
void setVerifyPeer(bool value);                        // default true
void ignoreSelfSignedCertificate();
void setTransferTimeout(const TimeSpan& value);        // Qt socket-level timeout
void setRequestCookies(const QList<QNetworkCookie>& value);
void setNetworkProxy(const QNetworkProxy& value);

// lifecycle
bool start(const TimeSpan& timeout = TimeSpan::zero());   // spawns the worker thread
bool waitForCompletion(const TimeSpan& timeout = TimeSpan::zero());
void abortOperation();

// results (read after success or operationComplete)
bool       success() const;
int        statusCode() const;
QString    reasonPhrase() const;
QByteArray responseBody() const;
QString    completionMessage() const;
TimeSpan   duration() const;
QNetworkReply::NetworkError networkError() const;
QList<QNetworkCookie>        responseCookies() const;

// signal
void operationComplete();
```

### `HttpGet` — `<Kanoop/http/httpget.h>`

GET with optional URL query parameters and optional streaming reads.

Synchronous:
```cpp
auto get = new HttpGet(url);
get->setVerifyPeer(profile.isSsl());
get->setTransferTimeout(TimeSpan::fromSeconds(30));
get->appendHeader(QNetworkRequest::ContentTypeHeader, "application/json");
if(token.isEmpty() == false) {
    get->appendHeader("Authorization", "Bearer " + token.toUtf8());
}
get->addParameter("filter", "active");          // appends as ?filter=active
get->addParameter("limit", "100");

if(get->start() && get->waitForCompletion(TimeSpan::fromSeconds(35))) {
    if(get->success() && get->statusCode() == 200) {
        const QByteArray body = get->responseBody();
        // parse JSON / consume body
    } else {
        Log::logText(LVL_WARNING, QString("GET %1 failed: %2")
            .arg(url, get->completionMessage()));
    }
}
delete get;
```

Asynchronous (pick this when the caller can't block):
```cpp
auto get = new HttpGet(url);
get->setVerifyPeer(true);
get->setTransferTimeout(TimeSpan::fromSeconds(30));
connect(get, &HttpOperation::operationComplete, this, [this, get]() {
    if(get->success() && get->statusCode() == 200) {
        handleResponse(get->responseBody());
    }
    get->deleteLater();
});
get->start();    // caller returns immediately; slot fires when the worker finishes
```

Streaming variant for large downloads:
```cpp
get->setStreamingRead(true);
connect(get, &HttpGet::dataAvailable,    this, &MyClass::onChunk);
connect(get, &HttpGet::downloadProgress, this, &MyClass::onProgress);
// When streaming, responseBody() is empty — consume chunks via dataAvailable.
```

### `HttpPost` — `<Kanoop/http/httppost.h>`

POST with either a raw `QByteArray` body or any object implementing `ISerializableToJson`. The JSON overload auto-serializes and sets `Content-Type: application/json` for you.
```cpp
// Raw body
auto post = new HttpPost(url, payloadBytes);
post->appendHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");

// JSON body (Content-Type already set by execute())
auto post = new HttpPost(url, myJsonObject);    // myJsonObject : ISerializableToJson
post->appendHeader("Authorization", bearer);

post->start();
if(post->waitForCompletion(TimeSpan::fromSeconds(35)) && post->success()) {
    // read post->responseBody()
}
delete post;
```
Pitfall: with the JSON overload, **do not** also append `Content-Type` — `execute()` sets it.

### `HttpPut` — `<Kanoop/http/httpput.h>`

Same shape as `HttpPost` — raw bytes or JSON-serializable body. Same Content-Type auto-set on the JSON overload.

### `HttpDelete` — `<Kanoop/http/httpdelete.h>`

DELETE, optionally with a request body (rare but RFC-allowed).
```cpp
auto del = new HttpDelete(url);
del->start();
del->waitForCompletion(TimeSpan::fromSeconds(30));
```

### `HttpUpload` — `<Kanoop/http/httpupload.h>`

Multipart/form-data file upload with form parameters and progress.
```cpp
auto up = new HttpUpload(url, "/path/to/firmware.swu");
up->addParameter("device_id", deviceId);
connect(up, &HttpUpload::uploadProgress, this,
        [](uint64_t sent, uint64_t total) { /* update UI */ });

up->start();
if(up->waitForCompletion(TimeSpan::fromSeconds(120)) && up->success()) {
    // upload complete
}
delete up;
```
Pitfall: the file is opened on the worker thread — make sure the path exists *before* `start()`. The file handle stays open for the whole upload; do not modify the file from the caller.

### `MqttClient` / `MqttParameters` — `<Kanoop/mqtt/mqttclient.h>`, `<Kanoop/mqtt/mqttparameters.h>`

Pub/sub wrapper around `QMqttClient` with auto-reconnect, TLS, and optional client certificates.
```cpp
MqttParameters params("broker.example.com", 8883);
params.setUsername("user");
params.setPassword("pass");
params.setUseSsl(true);
params.setClientCertificate(certPath);
params.setPrivateKey(keyPath);

MqttClient client(params);
client.setAutoReconnect(true);
client.setAutoReconnectTime(TimeSpan::fromSeconds(5));
client.start();

QMqttSubscription* sub = client.subscribe("site/+/telemetry", 1);
connect(sub, &QMqttSubscription::messageReceived, this, &MyClass::onMessage);

client.publish("site/123/command", payload, 1, /* retain */ false);

// shutdown
client.stop();
```
Pitfall: connection lifecycle is `start()`/`stop()`. After `stop()` the client must be restarted to reconnect — `setAutoReconnect` only governs *unexpected* disconnects.

## Common gotchas

- **Lifetime: never delete an `HttpOperation` while its worker is still running.** Always `waitForCompletion()` before `delete`, or wire `operationComplete` to a slot that deletes (and don't simultaneously block on `waitForCompletion`). Deleting a live op tears the QNetworkAccessManager out from under the worker thread.
- **`waitForCompletion` timeout must exceed `transferTimeout`.** If the socket-level transfer timeout is 30s but you only wait 30s, the worker may still be tearing down when you return. The canonical idiom (used by `CloudGet::getSynchronous`) is `waitForCompletion(transferTimeout + 2s)` or more.
- **`success() == true` does not mean 2xx.** It means the network layer completed without a `QNetworkReply` error. A 404 or 500 is a *successful* operation that returned a non-2xx status. Always check `statusCode()` separately.
- **JSON-overload Content-Type is auto-set.** `HttpPost(url, ISerializableToJson&)` and `HttpPut(url, ISerializableToJson&)` set `application/json` in `execute()`. Don't add it again — you'll end up with two headers.
- **No retries.** Operations are single-shot. Build a caller-side retry loop with backoff if you need it.
- **No client certificate support in `HttpOperation` base.** TAU/SGW REST integration that needs mutual TLS subclasses or overrides `configureSsl(QNetworkRequest*)`. If you need this in new code, look at how `CloudGet` / `CloudPost` are built — don't try to bolt it on at the call site.
- **Streaming reads suppress `responseBody`.** With `setStreamingRead(true)` on `HttpGet`, the body is delivered chunk by chunk via `dataAvailable`; `responseBody()` stays empty. Consume in the slot.
- **`addParameter` does different things on different classes.** On `HttpGet` it appends to the URL query string. On `HttpUpload` it adds a form field to the multipart body. Read the header before using.
- **Cookies and proxies, not headers.** Use `setRequestCookies` for cookies and `setNetworkProxy` for proxies — don't try to set `Cookie:` or `Proxy-*:` headers manually; the Qt request layer will overwrite or duplicate them.
- **Declspec on every new public class.** Missing `LIBKANOOPPROTOCOL_EXPORT` compiles on Linux and fails CI on Windows MinGW.

## Working in this repo

- When this library is consumed as a git submodule, the submodule will typically be in **detached HEAD** state pointing at a pinned commit. Check out a working branch before committing.
- Conventional-commits message format is used: `<type>(<scope>): <description>` with types `fix`, `feat`, `doc`, `refactor`, `test`, `chore`. Add a `Co-Authored-By` trailer if pair-programming.
- Submodule pointer changes belong in the consuming repo, not here.
