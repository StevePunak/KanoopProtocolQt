#include "http/httpoperation.h"
#include "httpstatuscodes.h"

#include <QNetworkAccessManager>

#include <Kanoop/commonexception.h>

const HttpOperation::RequestMethodToStringMap HttpOperation::_RequestMethodToStringMap;

HttpOperation::HttpOperation(const QString& url, RequestMethod method) :
    _url(url), _method(method)
{
}

HttpOperation::~HttpOperation()
{
    if(_networkAccessManager != nullptr) {
        delete _networkAccessManager;
    }
}

QNetworkAccessManager* HttpOperation::networkAccessManager()
{
    if(_networkAccessManager == nullptr) {
        _networkAccessManager = new QNetworkAccessManager(this);
    }
    return _networkAccessManager;
}

void HttpOperation::setReply(QNetworkReply* reply)
{
    connect(reply, &QNetworkReply::finished, this, &HttpOperation::onReplyFinished);
}

void HttpOperation::appendHeadersToRequest(QNetworkRequest* request)
{
    for(auto it = _headers.constBegin();it != _headers.constEnd();it++) {
        request->setHeader(it.key(), it.value());
    }
    for(auto it = _customHeaders.constBegin();it != _customHeaders.constEnd();it++) {
        request->setRawHeader(it.key().toUtf8(), it.value());
    }
    if(_requestCookies.count() > 0) {
        request->setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(_requestCookies));
    }
}

void HttpOperation::configureSsl(QNetworkRequest* request)
{
    if(isHttps()) {
        QSslConfiguration sslConfig = request->sslConfiguration();
        sslConfig.setPeerVerifyMode(_verifyPeer ? QSslSocket::VerifyPeer : QSslSocket::VerifyNone);
        request->setSslConfiguration(sslConfig);
    }
}

bool HttpOperation::isHttps() const
{
    QUrl url(_url);
    if(url.scheme() == "https") {
        return true;
    }
    return false;
}

void HttpOperation::threadStarted()
{
    try
    {
        _operationStartTime = QDateTime::currentDateTimeUtc();

        execute();
    }
    catch(const CommonException& e)
    {
        finishAndStop(false, e.message());
    }
}

void HttpOperation::threadFinished()
{
    emit operationComplete();
}

void HttpOperation::onReplyFinished()
{
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
    assert(reply);

    postReplyHook(reply);

    for(const QNetworkReply::RawHeaderPair& pair : reply->rawHeaderPairs()) {
        QString name = pair.first;
        QByteArray payload = pair.second;
        if(QString::compare(name, "set-cookie", Qt::CaseInsensitive) == 0) {
            _responseCookies = QNetworkCookie::parseCookies(payload);
        }
    }

    _networkError = reply->error();
    bool success = _networkError == QNetworkReply::NoError;
    QString message = success ? "Operation Successful" : reply->errorString();
    _statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    _reasonPhrase = HttpStatus::reasonPhrase(_statusCode);
    _responseBody = reply->readAll();

    if(_networkAccessManager != nullptr) {
        _networkAccessManager->deleteLater();
        _networkAccessManager = nullptr;
    }

    finishAndStop(success, message);
}

void HttpOperation::onReplyError(QNetworkReply::NetworkError error)
{
    _networkError = error;
}
