# KanoopProtocolQt {#mainpage}

Qt library providing threaded HTTP operations and an auto-reconnecting MQTT client.

## Modules

| Module | Description |
|--------|-------------|
| **HTTP** | Threaded GET, POST, PUT, DELETE, and multipart upload via `QNetworkAccessManager` |
| **MQTT** | Auto-reconnecting MQTT 5.0 / 3.1.1 client with SSL/TLS support |

## Quick Start

### HTTP GET

```cpp
#include <Kanoop/http/httpget.h>

HttpGet get("https://api.example.com/data");
get.addParameter("page", "1");
connect(&get, &HttpOperation::operationComplete, [&]() {
    qDebug() << get.statusCode() << get.responseBody();
});
get.start(); // runs in a dedicated thread
```

### HTTP POST (JSON)

```cpp
#include <Kanoop/http/httppost.h>

QByteArray json = R"({"name":"test"})";
HttpPost post("https://api.example.com/items", json);
post.appendHeader(QNetworkRequest::ContentTypeHeader, "application/json");
connect(&post, &HttpOperation::operationComplete, [&]() {
    qDebug() << post.statusCode();
});
post.start();
```

### MQTT Client

```cpp
#include <Kanoop/mqtt/mqttclient.h>
#include <Kanoop/mqtt/mqttparameters.h>

MqttParameters params("broker.example.com", 8883);
params.setUseSsl(true);
params.setUsername("device-001");
params.setPassword("token");

MqttClient client(params);
connect(&client, &MqttClient::clientConnected, [&]() {
    client.subscribe("sensors/temperature");
    client.publish("status/online", "1");
});
client.start();
```

## Building

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
cmake --build build --parallel
```

Requires [KanoopCommonQt](https://github.com/StevePunak/KanoopCommonQt) as a dependency.

## Links

- [GitHub Repository](https://github.com/StevePunak/KanoopProtocolQt)
- [Class List](annotated.html)
- [Class Hierarchy](hierarchy.html)
- [File List](files.html)
