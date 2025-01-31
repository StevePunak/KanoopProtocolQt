#ifndef HTTPOPERATION_H
#define HTTPOPERATION_H
#include <Kanoop/utility/abstractthreadclass.h>
#include <Kanoop/http/httpheaders.h>

#include <QNetworkCookie>
#include <QNetworkReply>

class QNetworkAccessManager;
class HttpOperation : public AbstractThreadClass
{
    Q_OBJECT
protected:
    HttpOperation(const QString& url);
public:
    virtual ~HttpOperation();

    QString url() const { return _url; }

    HttpKnownHeaders headers() const { return _headers; }
    HttpCustomHeaders customHeaders() const { return _customHeaders; }
    void appendHeader(QNetworkRequest::KnownHeaders type, const QByteArray& value) { _headers.insert(type, value); }
    void appendHeader(const QString& headerName, const QByteArray& value) { _customHeaders.insert(headerName, value); }

    QList<QNetworkCookie> requestCookies() const { return _requestCookies; }
    void setRequestCookies(const QList<QNetworkCookie>& value) { _requestCookies = value; }

    QList<QNetworkCookie> responseCookies() const { return _responseCookies; }

    bool isVerifyPeer() const { return _verifyPeer; }
    void setVerifyPeer(bool value) { _verifyPeer = value; }

    QNetworkReply::NetworkError networkError() const { return _networkError; }
    QString reasonPhrase() const { return _reasonPhrase; }
    QByteArray responseBody() const { return _responseBody; }
    TimeSpan duration() const { return _duration; }

    bool isHttps() const;

protected:
    virtual void execute() = 0;
    virtual void postReplyHook(QNetworkReply*) {}

    QNetworkAccessManager* networkAccessManager();

    void setReply(QNetworkReply* reply);

    void appendHeadersToRequest(QNetworkRequest* request);
    void configureSsl(QNetworkRequest* request);

private:
    virtual void threadStarted() override;
    virtual void threadFinished() override;

    QString _url;
    HttpKnownHeaders _headers;
    HttpCustomHeaders _customHeaders;
    QList<QNetworkCookie> _requestCookies;

    int _statusCode;
    QNetworkReply::NetworkError _networkError = QNetworkReply::UnknownServerError;
    QString _reasonPhrase;
    QByteArray _responseBody;
    QList<QNetworkCookie> _responseCookies;

    bool _verifyPeer = true;

    QDateTime _operationStartTime;
    TimeSpan _duration;

    QNetworkAccessManager* _networkAccessManager = nullptr;

signals:
    void operationComplete();

protected slots:
    void onReplyFinished();
    void onReplyError(QNetworkReply::NetworkError error);
};

#endif // HTTPOPERATION_H
