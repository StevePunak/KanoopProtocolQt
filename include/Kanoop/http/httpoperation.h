#ifndef HTTPOPERATION_H
#define HTTPOPERATION_H
#include <Kanoop/utility/abstractthreadclass.h>
#include <Kanoop/http/httpheaders.h>

#include <QNetworkCookie>
#include <QNetworkReply>
#include <Kanoop/kanoopprotocol.h>

class QNetworkAccessManager;
class LIBKANOOPPROTOCOL_EXPORT HttpOperation : public AbstractThreadClass
{
    Q_OBJECT
public:
    enum RequestMethod {
        UnknownMethod   = 0x0000,
        Get             = 0x0001,
        Put             = 0x0002,
        Delete          = 0x0004,
        Post            = 0x0008,
        Head            = 0x0010,
        Patch           = 0x0040,
        MultipartUpload = 0x1000,
    };

protected:
    HttpOperation(const QString& url, RequestMethod method/* = UnknownMethod*/);

public:
    virtual ~HttpOperation();

    QString url() const { return _url; }

    HttpKnownHeaders headers() const { return _headers; }
    HttpCustomHeaders customHeaders() const { return _customHeaders; }
    void appendHeader(QNetworkRequest::KnownHeaders type, const QByteArray& value) { _headers.insert(type, value); }
    void appendHeader(const QString& headerName, const QByteArray& value) { _customHeaders.insert(headerName, value); }

    void abortOperation();

    QList<QNetworkCookie> requestCookies() const { return _requestCookies; }
    void setRequestCookies(const QList<QNetworkCookie>& value) { _requestCookies = value; }

    QList<QNetworkCookie> responseCookies() const { return _responseCookies; }

    bool isVerifyPeer() const { return _verifyPeer; }
    void setVerifyPeer(bool value) { _verifyPeer = value; }

    TimeSpan transferTimeout() const { return _transferTimeout; }
    void setTransferTimeout(const TimeSpan& value) { _transferTimeout = value; }

    QNetworkReply::NetworkError networkError() const { return _networkError; }
    QString reasonPhrase() const { return _reasonPhrase; }
    QByteArray responseBody() const { return _responseBody; }
    int statusCode() const { return _statusCode; }
    TimeSpan duration() const { return _duration; }

    QString getRequestMethodString() const { return _RequestMethodToStringMap.getString(_method); }

    bool isHttps() const;

    static QString getRequestMethodString(RequestMethod method) { return _RequestMethodToStringMap.getString(method); }

protected:
    virtual void execute() = 0;
    virtual void postReplyHook(QNetworkReply*) {}
    void setUrl(const QString& value) { _url = value; }

    QNetworkAccessManager* networkAccessManager();

    void setReply(QNetworkReply* reply);

    void appendHeadersToRequest(QNetworkRequest* request);
    void configureSsl(QNetworkRequest* request);

    virtual void threadFinished() override;

private:
    virtual void threadStarted() override;

    QString _url;
    RequestMethod _method;

    HttpKnownHeaders _headers;
    HttpCustomHeaders _customHeaders;
    QList<QNetworkCookie> _requestCookies;

    int _statusCode = 0;
    QNetworkReply::NetworkError _networkError = QNetworkReply::UnknownServerError;
    QString _reasonPhrase;
    QByteArray _responseBody;
    QList<QNetworkCookie> _responseCookies;

    bool _verifyPeer = true;

    QDateTime _operationStartTime;
    TimeSpan _transferTimeout = TimeSpan::fromMilliseconds(QNetworkRequest::DefaultTransferTimeoutConstant);
    TimeSpan _duration;

    QNetworkAccessManager* _networkAccessManager = nullptr;
    QNetworkReply* _reply = nullptr;

    class RequestMethodToStringMap : public KANOOP::EnumToStringMap<RequestMethod>
    {
    public:
        RequestMethodToStringMap()
        {
            insert(UnknownMethod,   "UnknownMethod");
            insert(Get,             "GET");
            insert(Put,             "PUT");
            insert(Delete,          "DELETE");
            insert(Post,            "POST");
            insert(Head,            "HEAD");
            insert(Patch,           "PATCH");
            insert(MultipartUpload, "UPLOAD");
        }
    };

    static const RequestMethodToStringMap _RequestMethodToStringMap;

signals:
    void operationComplete();

protected slots:
    void onReplyFinished();
    void onReplyError(QNetworkReply::NetworkError error);
};

#endif // HTTPOPERATION_H
