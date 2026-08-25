#include <QTest>
#include <QNetworkRequest>
#include <QSslCertificate>

#include <Kanoop/http/httpheaders.h>
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/http/httpget.h>
#include <Kanoop/http/httppost.h>
#include <Kanoop/http/httpput.h>
#include <Kanoop/http/httpdelete.h>
#include <Kanoop/http/httpupload.h>
#include <Kanoop/timespan.h>

#include <Kanoop/http/httpstatuscodes.h>

// Throwaway fixture chain for SC-6036 (leaf issued by the sub-CA below). Not a
// real credential and never presented to anything — it exists so the chain
// accessors are exercised against non-null, correctly ordered certificates.
static const char* const FixtureLeafPem =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBfjCCASSgAwIBAgIUO/l0EsMloCIzj9c7QvzK8Uoh27YwCgYIKoZIzj0EAwIw\n"
    "HTEbMBkGA1UEAwwSU0M2MDM2IFRlc3QgU3ViLUNBMCAXDTI2MDgwMzIxNTcwN1oY\n"
    "DzIxMjYwNzEwMjE1NzA3WjAbMRkwFwYDVQQDDBBTQzYwMzYgVGVzdCBMZWFmMFkw\n"
    "EwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE8OHKWrpYjxqWlBx30cQ8z0Xjq0qnNBnb\n"
    "lR5/kRRsdfulbCGb55JrvsVUcuWUgGX/Vwx1WmxEvyNgFb7jaoC8QaNCMEAwHQYD\n"
    "VR0OBBYEFFk/XwxYR0wl68lMkucaD5/MvvlMMB8GA1UdIwQYMBaAFPch93e7EccG\n"
    "NgQcVD0my4dUvGAuMAoGCCqGSM49BAMCA0gAMEUCIDpxMCbP6d3ex/7tNBcrC6dm\n"
    "OpvK7qfl7m118JkoJF6/AiEA+l8TZ4jeNWFNUM2sPLC3EPjMX/bOgIjPKnCN5Q2x\n"
    "bfc=\n"
    "-----END CERTIFICATE-----\n";
static const char* const FixtureSubCaPem =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBkTCCATegAwIBAgIUKycV5QWXbhRnzcbmtpr9Pap5TbEwCgYIKoZIzj0EAwIw\n"
    "HTEbMBkGA1UEAwwSU0M2MDM2IFRlc3QgU3ViLUNBMCAXDTI2MDgwMzIxNTcwN1oY\n"
    "DzIxMjYwNzEwMjE1NzA3WjAdMRswGQYDVQQDDBJTQzYwMzYgVGVzdCBTdWItQ0Ew\n"
    "WTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAATD/vRUOTNQNu/wsQFmF2KZmZAhkBHg\n"
    "R6pFMRKq6YBolZsjn3QAhXHxX7M2/NeM+JGEP+Z8FXO6F91N6ypFqVVOo1MwUTAd\n"
    "BgNVHQ4EFgQU9yH3d7sRxwY2BBxUPSbLh1S8YC4wHwYDVR0jBBgwFoAU9yH3d7sR\n"
    "xwY2BBxUPSbLh1S8YC4wDwYDVR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAgNIADBF\n"
    "AiAGkRxP5yLe7i8Bf3H1zWnBPzqLi5td318iedUYbK/91QIhAM78XWMux56TyUE6\n"
    "GcqJi/j/QORfOPEiItq764q5y2jL\n"
    "-----END CERTIFICATE-----\n";

// Throwaway private key paired with no particular certificate above — configureSsl()
// only checks that a private key is present, it never validates that the key matches
// the leaf, so any parseable EC key satisfies the guard.
static const char* const FixtureLeafKeyPem =
    "-----BEGIN EC PRIVATE KEY-----\n"
    "MHcCAQEEIE9P4mhT9QdH20NEr6//JiSNMpva11PS+aV2/2dHPRBYoAoGCCqGSM49\n"
    "AwEHoUQDQgAEcPmodQTBJcpp+v/wnvAwbzRkKkgir1m0jSTZIkl0c0XWCoC5gEv/\n"
    "YR7BJ79ICvj1eblw5DIgxyNyeIJbAj3OUw==\n"
    "-----END EC PRIVATE KEY-----\n";

static QList<QSslCertificate> fixtureChain()
{
    QList<QSslCertificate> chain;
    chain.append(QSslCertificate(QByteArray(FixtureLeafPem), QSsl::Pem));
    chain.append(QSslCertificate(QByteArray(FixtureSubCaPem), QSsl::Pem));
    return chain;
}

// Test-local subclass exposing the protected configureSsl() so it can be pinned
// directly rather than only indirectly through execute().
class SslConfiguringHttpGet : public HttpGet
{
public:
    explicit SslConfiguringHttpGet(const QString& url) : HttpGet(url) {}

    void callConfigureSsl(QNetworkRequest* request) { configureSsl(request); }
};

class TstHttp : public QObject
{
    Q_OBJECT

private slots:
    // ---- HttpCustomHeaders ----

    void customHeaders_insertAndRetrieve()
    {
        HttpCustomHeaders headers;
        headers.insert("X-Api-Key", "abc123");
        headers.insert("X-Request-Id", "req-42");

        QCOMPARE(headers.count(), 2);
        QCOMPARE(headers.value("X-Api-Key"), QByteArray("abc123"));
        QCOMPARE(headers.value("X-Request-Id"), QByteArray("req-42"));
    }

    void customHeaders_empty()
    {
        HttpCustomHeaders headers;
        QVERIFY(headers.isEmpty());
        QCOMPARE(headers.count(), 0);
    }

    void customHeaders_overwrite()
    {
        HttpCustomHeaders headers;
        headers.insert("X-Token", "old");
        headers.insert("X-Token", "new");
        QCOMPARE(headers.value("X-Token"), QByteArray("new"));
    }

    // ---- HttpKnownHeaders ----

    void knownHeaders_insertAndRetrieve()
    {
        HttpKnownHeaders headers;
        headers.insert(QNetworkRequest::ContentTypeHeader, "application/json");
        headers.insert(QNetworkRequest::UserAgentHeader, "TestAgent/1.0");

        QCOMPARE(headers.count(), 2);
        QCOMPARE(headers.value(QNetworkRequest::ContentTypeHeader), QByteArray("application/json"));
    }

    // ---- HttpOperation (via HttpGet since base is abstract) ----

    void httpGet_urlAccessor()
    {
        HttpGet get("https://api.example.com/data");
        QCOMPARE(get.url(), QString("https://api.example.com/data"));
    }

    void httpGet_isHttps()
    {
        HttpGet https("https://secure.example.com");
        QVERIFY(https.isHttps());

        HttpGet http("http://plain.example.com");
        QVERIFY(!http.isHttps());
    }

    void httpGet_defaultStatusCode()
    {
        HttpGet get("http://example.com");
        QCOMPARE(get.statusCode(), 0);
    }

    void httpGet_defaultNetworkError()
    {
        HttpGet get("http://example.com");
        QCOMPARE(get.networkError(), QNetworkReply::UnknownServerError);
    }

    void httpGet_appendKnownHeader()
    {
        HttpGet get("http://example.com");
        get.appendHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

        HttpKnownHeaders h = get.headers();
        QCOMPARE(h.count(), 1);
        QCOMPARE(h.value(QNetworkRequest::ContentTypeHeader), QByteArray("text/plain"));
    }

    void httpGet_appendCustomHeader()
    {
        HttpGet get("http://example.com");
        get.appendHeader("Authorization", "Bearer tok123");

        HttpCustomHeaders h = get.customHeaders();
        QCOMPARE(h.count(), 1);
        QCOMPARE(h.value("Authorization"), QByteArray("Bearer tok123"));
    }

    void httpGet_multipleHeaders()
    {
        HttpGet get("http://example.com");
        get.appendHeader("X-First", "1");
        get.appendHeader("X-Second", "2");
        get.appendHeader(QNetworkRequest::UserAgentHeader, "Test");

        QCOMPARE(get.customHeaders().count(), 2);
        QCOMPARE(get.headers().count(), 1);
    }

    void httpGet_verifyPeerDefault()
    {
        HttpGet get("https://example.com");
        QVERIFY(get.isVerifyPeer()); // default is true
    }

    void httpGet_setVerifyPeer()
    {
        HttpGet get("https://example.com");
        get.setVerifyPeer(false);
        QVERIFY(!get.isVerifyPeer());
    }

    void httpGet_selfSignedCertIgnore()
    {
        HttpGet get("https://example.com");
        QVERIFY(!get.isSelfSignedCertificateErrorIgnored());
        get.ignoreSelfSignedCertificate();
        QVERIFY(get.isSelfSignedCertificateErrorIgnored());
    }

    void httpGet_transferTimeout()
    {
        HttpGet get("http://example.com");
        get.setTransferTimeout(TimeSpan::fromSeconds(30));
        QCOMPARE(get.transferTimeout().totalSeconds(), 30.0);
    }

    void httpGet_cookies()
    {
        HttpGet get("http://example.com");
        QVERIFY(get.requestCookies().isEmpty());

        QList<QNetworkCookie> cookies;
        cookies.append(QNetworkCookie("session", "abc"));
        get.setRequestCookies(cookies);

        QCOMPARE(get.requestCookies().count(), 1);
        QCOMPARE(get.requestCookies().first().name(), QByteArray("session"));
    }

    void httpGet_streamingReadDefault()
    {
        HttpGet get("http://example.com");
        QVERIFY(!get.streamingRead());
    }

    void httpGet_setStreamingRead()
    {
        HttpGet get("http://example.com");
        get.setStreamingRead(true);
        QVERIFY(get.streamingRead());
    }

    // ---- RequestMethod string mapping ----

    void requestMethodString_get()
    {
        QCOMPARE(HttpOperation::getRequestMethodString(HttpOperation::Get), QString("GET"));
    }

    void requestMethodString_post()
    {
        QCOMPARE(HttpOperation::getRequestMethodString(HttpOperation::Post), QString("POST"));
    }

    void requestMethodString_put()
    {
        QCOMPARE(HttpOperation::getRequestMethodString(HttpOperation::Put), QString("PUT"));
    }

    void requestMethodString_delete()
    {
        QCOMPARE(HttpOperation::getRequestMethodString(HttpOperation::Delete), QString("DELETE"));
    }

    void requestMethodString_head()
    {
        QCOMPARE(HttpOperation::getRequestMethodString(HttpOperation::Head), QString("HEAD"));
    }

    void requestMethodString_patch()
    {
        QCOMPARE(HttpOperation::getRequestMethodString(HttpOperation::Patch), QString("PATCH"));
    }

    void requestMethodString_upload()
    {
        QCOMPARE(HttpOperation::getRequestMethodString(HttpOperation::MultipartUpload), QString("UPLOAD"));
    }

    void requestMethodString_instance()
    {
        HttpGet get("http://example.com");
        QCOMPARE(get.getRequestMethodString(), QString("GET"));
    }

    // ---- HttpPost construction ----

    void httpPost_urlAndBody()
    {
        HttpPost post("http://example.com/api", QByteArray("{\"key\":\"val\"}"));
        QCOMPARE(post.url(), QString("http://example.com/api"));
        QCOMPARE(post.requestBody(), QByteArray("{\"key\":\"val\"}"));
    }

    void httpPost_emptyBody()
    {
        HttpPost post("http://example.com/api");
        QCOMPARE(post.requestBody(), QByteArray());
    }

    // ---- HttpPut construction ----

    void httpPut_urlAndBody()
    {
        HttpPut put("http://example.com/resource", QByteArray("data"));
        QCOMPARE(put.url(), QString("http://example.com/resource"));
        QCOMPARE(put.putBody(), QByteArray("data"));
    }

    // ---- HttpUpload construction ----

    void httpUpload_urlAndFilename()
    {
        HttpUpload upload("http://example.com/upload", "/tmp/test.bin");
        QCOMPARE(upload.url(), QString("http://example.com/upload"));
        QCOMPARE(upload.filename(), QString("/tmp/test.bin"));
    }

    void httpUpload_setFilename()
    {
        HttpUpload upload("http://example.com/upload");
        QVERIFY(upload.filename().isEmpty());
        upload.setFilename("/tmp/other.bin");
        QCOMPARE(upload.filename(), QString("/tmp/other.bin"));
    }

    // ---- HttpStatus utility functions ----

    void httpStatus_isInformational()
    {
        QVERIFY(HttpStatus::isInformational(100));
        QVERIFY(HttpStatus::isInformational(103));
        QVERIFY(!HttpStatus::isInformational(200));
    }

    void httpStatus_isSuccessful()
    {
        QVERIFY(HttpStatus::isSuccessful(200));
        QVERIFY(HttpStatus::isSuccessful(201));
        QVERIFY(HttpStatus::isSuccessful(204));
        QVERIFY(!HttpStatus::isSuccessful(301));
    }

    void httpStatus_isRedirection()
    {
        QVERIFY(HttpStatus::isRedirection(301));
        QVERIFY(HttpStatus::isRedirection(302));
        QVERIFY(!HttpStatus::isRedirection(200));
    }

    void httpStatus_isClientError()
    {
        QVERIFY(HttpStatus::isClientError(400));
        QVERIFY(HttpStatus::isClientError(404));
        QVERIFY(HttpStatus::isClientError(401));
        QVERIFY(!HttpStatus::isClientError(500));
    }

    void httpStatus_isServerError()
    {
        QVERIFY(HttpStatus::isServerError(500));
        QVERIFY(HttpStatus::isServerError(503));
        QVERIFY(!HttpStatus::isServerError(404));
    }

    void httpStatus_isError()
    {
        QVERIFY(HttpStatus::isError(400));
        QVERIFY(HttpStatus::isError(500));
        QVERIFY(!HttpStatus::isError(200));
        QVERIFY(!HttpStatus::isError(301));
    }

    void httpStatus_reasonPhrase()
    {
        QCOMPARE(HttpStatus::reasonPhrase(200), QString("OK"));
        QCOMPARE(HttpStatus::reasonPhrase(404), QString("Not Found"));
        QCOMPARE(HttpStatus::reasonPhrase(500), QString("Internal Server Error"));
        QCOMPARE(HttpStatus::reasonPhrase(201), QString("Created"));
    }

    // ---- HttpOperation client certificate chain (SC-6036) ----

    void localCertificateChainRoundTripsLeafFirst()
    {
        const QList<QSslCertificate> chain = fixtureChain();
        QVERIFY(chain.at(0).isNull() == false);      // fixture parsed
        QVERIFY(chain.at(1).isNull() == false);

        HttpGet op("https://example.com");
        op.setLocalCertificateChain(chain);

        QCOMPARE(op.localCertificateChain().count(), 2);
        QCOMPARE(op.localCertificateChain(), chain);
        // localCertificate() must be the LEAF, not the sub-CA: Qt presents the chain
        // leaf-first, and a reversed chain fails path building at the peer.
        QCOMPARE(op.localCertificate(), chain.at(0));
        QCOMPARE(op.localCertificate().subjectInfo(QSslCertificate::CommonName).value(0),
                 QStringLiteral("SC6036 Test Leaf"));
    }

    void setLocalCertificateNormalizesToSingleElementChain()
    {
        const QSslCertificate leaf = fixtureChain().at(0);
        QVERIFY(leaf.isNull() == false);

        HttpGet op("https://example.com");
        op.setLocalCertificate(leaf);

        QCOMPARE(op.localCertificateChain().count(), 1);
        QCOMPARE(op.localCertificateChain().at(0), leaf);
        QCOMPARE(op.localCertificate(), leaf);
    }

    void setLocalCertificateWithNullYieldsEmptyChain()
    {
        HttpGet op("https://example.com");
        op.setLocalCertificate(QSslCertificate());
        QVERIFY2(op.localCertificateChain().isEmpty(),
                 "a null certificate must yield an empty chain, not a one-element chain holding a null");
        QVERIFY(op.localCertificate().isNull());
    }

    // ---- HttpOperation::configureSsl (SC-6036) ----

    void configureSslPresentsFullChainToQt()
    {
        const QList<QSslCertificate> chain = fixtureChain();
        const QSslKey key(QByteArray(FixtureLeafKeyPem), QSsl::Ec, QSsl::Pem, QSsl::PrivateKey);
        QVERIFY(key.isNull() == false);

        SslConfiguringHttpGet op("https://example.com");
        op.setLocalCertificateChain(chain);
        op.setPrivateKey(key);

        QNetworkRequest request;
        op.callConfigureSsl(&request);

        const QList<QSslCertificate> presented = request.sslConfiguration().localCertificateChain();
        QCOMPARE(presented.count(), 2);
        QCOMPARE(presented.at(0).subjectInfo(QSslCertificate::CommonName).value(0),
                 QStringLiteral("SC6036 Test Leaf"));
        QCOMPARE(presented.at(1).subjectInfo(QSslCertificate::CommonName).value(0),
                 QStringLiteral("SC6036 Test Sub-CA"));
    }

    void configureSslWithNullLeafDoesNotEngageMtls()
    {
        const QList<QSslCertificate> nullLeafChain = QList<QSslCertificate>() << QSslCertificate();
        const QSslKey key(QByteArray(FixtureLeafKeyPem), QSsl::Ec, QSsl::Pem, QSsl::PrivateKey);
        QVERIFY(key.isNull() == false);

        SslConfiguringHttpGet op("https://example.com");
        op.setLocalCertificateChain(nullLeafChain);
        op.setPrivateKey(key);

        QNetworkRequest request;
        op.callConfigureSsl(&request);

        QVERIFY2(request.sslConfiguration().localCertificateChain().isEmpty(),
                 "a one-element chain holding a null certificate must not engage mTLS");
    }

    void configureSslWithEmptyChainDoesNotEngageMtls()
    {
        const QSslKey key(QByteArray(FixtureLeafKeyPem), QSsl::Ec, QSsl::Pem, QSsl::PrivateKey);
        QVERIFY(key.isNull() == false);

        SslConfiguringHttpGet op("https://example.com");
        op.setLocalCertificateChain(QList<QSslCertificate>());
        op.setPrivateKey(key);

        QNetworkRequest request;
        op.callConfigureSsl(&request);

        QVERIFY2(request.sslConfiguration().localCertificateChain().isEmpty(),
                 "an empty chain must not engage mTLS");
    }
};

QTEST_MAIN(TstHttp)
#include "tst_http.moc"
