#include <QTest>
#include <QSignalSpy>
#include <QJsonDocument>
#include <QJsonObject>

#include <Kanoop/http/httpget.h>
#include <Kanoop/http/httppost.h>
#include <Kanoop/http/httpput.h>
#include <Kanoop/http/httpdelete.h>
#include <Kanoop/timespan.h>

// This suite makes LIVE network calls to https://httpbin.org (a public third-party
// service) to exercise HttpGet/HttpPost/HttpPut/HttpDelete end-to-end over real TLS.
// It is opt-in and gated behind KANOOP_RUN_NETWORK_TESTS so that ordinary `ctest`
// runs (including CI) are deterministic: a flaky internet connection or httpbin.org
// downtime must never fail a PR that has nothing to do with HTTP code. See the
// SC-6036 follow-up notes for why this was added.
//
// Run it deliberately with:
//   KANOOP_RUN_NETWORK_TESTS=1 ctest -R tst_http_integration --output-on-failure
//
// Even when opted in, initTestCase() probes httpbin.org once before running any
// test. If that probe cannot connect (DNS failure, timeout, connection refused),
// the whole suite is skipped rather than failed, so a network blip on a nightly
// run also shows up as SKIP, not FAIL. An unexpected HTTP status code or a bad
// response body from a server that IS reachable is a real regression and is
// never turned into a skip by this gate.
//
// Do not remove this gate to "fix" a CI failure without first confirming the
// failure is an actual code regression rather than network noise.
static const QString BASE_URL = "https://httpbin.org";

// Helper: run an HttpOperation synchronously with a timeout
static bool waitForComplete(HttpOperation* op, int timeoutMs = 10000)
{
    QSignalSpy spy(op, &HttpOperation::operationComplete);
    op->start();
    return spy.wait(timeoutMs);
}

class TstHttpIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        if(qEnvironmentVariableIsSet("KANOOP_RUN_NETWORK_TESTS") == false) {
            QSKIP("Live network tests are opt-in. Set KANOOP_RUN_NETWORK_TESTS=1 to run them.");
        }

        // Reachability probe: a connect/timeout-class failure here means the
        // network (or httpbin.org) is unavailable, not that the code under
        // test regressed, so skip the whole suite rather than fail it.
        HttpGet probe(BASE_URL + "/get");
        probe.setVerifyPeer(false);
        probe.setTransferTimeout(TimeSpan::fromSeconds(5));
        if(waitForComplete(&probe, 5000) == false || probe.networkError() != QNetworkReply::NoError) {
            QSKIP(qPrintable(QString("httpbin.org is unreachable (network error %1); skipping live network tests")
                              .arg(static_cast<int>(probe.networkError()))));
        }
    }

    // ---- GET ----

    void get_basic()
    {
        HttpGet get(BASE_URL + "/get");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 200);
        QVERIFY(!get.responseBody().isEmpty());

        QJsonObject json = QJsonDocument::fromJson(get.responseBody()).object();
        QVERIFY(json.contains("url"));
        QCOMPARE(json["url"].toString(), BASE_URL + "/get");
    }

    void get_withQueryParameters()
    {
        HttpGet get(BASE_URL + "/get");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));
        get.addParameter("foo", "bar");
        get.addParameter("count", "42");

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 200);

        QJsonObject json = QJsonDocument::fromJson(get.responseBody()).object();
        QJsonObject args = json["args"].toObject();
        QCOMPARE(args["foo"].toString(), QString("bar"));
        QCOMPARE(args["count"].toString(), QString("42"));
    }

    void get_customHeaders()
    {
        HttpGet get(BASE_URL + "/headers");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));
        get.appendHeader("X-Custom-Test", "hello123");

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 200);

        QJsonObject json = QJsonDocument::fromJson(get.responseBody()).object();
        QJsonObject headers = json["headers"].toObject();
        QCOMPARE(headers["X-Custom-Test"].toString(), QString("hello123"));
    }

    void get_404()
    {
        HttpGet get(BASE_URL + "/status/404");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 404);
    }

    void get_responseHeaders()
    {
        HttpGet get(BASE_URL + "/response-headers?X-Test-Header=works");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 200);
    }

    void get_duration()
    {
        HttpGet get(BASE_URL + "/get");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 200);

        // Duration should be positive and less than the timeout
        QVERIFY(get.duration().totalMilliseconds() > 0);
        QVERIFY(get.duration().totalSeconds() < 15.0);
    }

    // ---- POST ----

    void post_jsonBody()
    {
        QJsonObject payload;
        payload["name"] = "test";
        payload["value"] = 42;
        QByteArray body = QJsonDocument(payload).toJson(QJsonDocument::Compact);

        HttpPost post(BASE_URL + "/post", body);
        post.setVerifyPeer(false);
        post.setTransferTimeout(TimeSpan::fromSeconds(15));
        post.appendHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QVERIFY(waitForComplete(&post));
        QCOMPARE(post.statusCode(), 200);

        QJsonObject json = QJsonDocument::fromJson(post.responseBody()).object();
        // httpbin echoes the posted JSON in the "json" field
        QJsonObject echoed = json["json"].toObject();
        QCOMPARE(echoed["name"].toString(), QString("test"));
        QCOMPARE(echoed["value"].toInt(), 42);
    }

    void post_emptyBody()
    {
        HttpPost post(BASE_URL + "/post");
        post.setVerifyPeer(false);
        post.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&post));
        QCOMPARE(post.statusCode(), 200);
    }

    void post_rawBody()
    {
        QByteArray rawData("Hello, httpbin!");
        HttpPost post(BASE_URL + "/post", rawData);
        post.setVerifyPeer(false);
        post.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&post));
        QCOMPARE(post.statusCode(), 200);

        QJsonObject json = QJsonDocument::fromJson(post.responseBody()).object();
        QCOMPARE(json["data"].toString(), QString("Hello, httpbin!"));
    }

    // ---- PUT ----

    void put_jsonBody()
    {
        QJsonObject payload;
        payload["updated"] = true;
        QByteArray body = QJsonDocument(payload).toJson(QJsonDocument::Compact);

        HttpPut put(BASE_URL + "/put", body);
        put.setVerifyPeer(false);
        put.setTransferTimeout(TimeSpan::fromSeconds(15));
        put.appendHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QVERIFY(waitForComplete(&put));
        QCOMPARE(put.statusCode(), 200);

        QJsonObject json = QJsonDocument::fromJson(put.responseBody()).object();
        QJsonObject echoed = json["json"].toObject();
        QCOMPARE(echoed["updated"].toBool(), true);
    }

    // ---- DELETE ----

    void delete_basic()
    {
        HttpDelete del(BASE_URL + "/delete");
        del.setVerifyPeer(false);
        del.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&del));
        QCOMPARE(del.statusCode(), 200);

        QJsonObject json = QJsonDocument::fromJson(del.responseBody()).object();
        QVERIFY(json.contains("url"));
    }

    void delete_withBody()
    {
        QByteArray body("{\"id\":99}");
        HttpDelete del(BASE_URL + "/delete", body);
        del.setVerifyPeer(false);
        del.setTransferTimeout(TimeSpan::fromSeconds(15));
        del.appendHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QVERIFY(waitForComplete(&del));
        QCOMPARE(del.statusCode(), 200);

        QJsonObject json = QJsonDocument::fromJson(del.responseBody()).object();
        QCOMPARE(json["json"].toObject()["id"].toInt(), 99);
    }

    // ---- Status codes ----

    void statusCodes_200()
    {
        HttpGet get(BASE_URL + "/status/200");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 200);
    }

    void statusCodes_201()
    {
        HttpGet get(BASE_URL + "/status/201");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 201);
    }

    void statusCodes_500()
    {
        HttpGet get(BASE_URL + "/status/500");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 500);
    }

    // ---- Cookies ----

    void cookies_sendWithRequest()
    {
        // Send cookies with the request and verify httpbin echoes them back
        HttpGet get(BASE_URL + "/cookies");
        get.setVerifyPeer(false);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QList<QNetworkCookie> cookies;
        cookies.append(QNetworkCookie("session", "abc123"));
        cookies.append(QNetworkCookie("lang", "en"));
        get.setRequestCookies(cookies);

        QVERIFY(waitForComplete(&get));
        QCOMPARE(get.statusCode(), 200);

        QJsonObject json = QJsonDocument::fromJson(get.responseBody()).object();
        QJsonObject echoed = json["cookies"].toObject();
        QCOMPARE(echoed["session"].toString(), QString("abc123"));
        QCOMPARE(echoed["lang"].toString(), QString("en"));
    }

    // ---- HTTPS ----

    void https_verifyPeerEnabled()
    {
        HttpGet get(BASE_URL + "/get");
        get.setVerifyPeer(true);
        get.setTransferTimeout(TimeSpan::fromSeconds(15));

        QVERIFY(get.isHttps());
        QVERIFY(waitForComplete(&get));
        // httpbin.org has a valid cert, so this should succeed
        QCOMPARE(get.statusCode(), 200);
    }
};

QTEST_MAIN(TstHttpIntegration)
#include "tst_http_integration.moc"
