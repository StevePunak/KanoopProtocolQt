#ifndef HTTPOPERATION_H
#define HTTPOPERATION_H
#include <Kanoop/utility/abstractthreadclass.h>
#include <Kanoop/http/httpheaders.h>

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

    QNetworkReply::NetworkError networkError() const { return _networkError; }
    QString reasonPhrase() const { return _reasonPhrase; }
    QByteArray responseBody() const { return _responseBody; }
    TimeSpan duration() const { return _duration; }

protected:
    virtual void execute() = 0;

    QNetworkAccessManager* networkAccessManager();
    void setReply(QNetworkReply* reply);
    void appendHeadersToRequest(QNetworkRequest* request);

private:
    virtual void threadStarted() override;
    virtual void threadFinished() override;

    QString _url;
    HttpKnownHeaders _headers;
    HttpCustomHeaders _customHeaders;

    int _statusCode;
    QNetworkReply::NetworkError _networkError = QNetworkReply::UnknownServerError;
    QString _reasonPhrase;
    QByteArray _responseBody;

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
