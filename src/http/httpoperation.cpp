#include "http/httpoperation.h"
#include <Kanoop/http/httpstatuscodes.h>

#include <QNetworkAccessManager>
#include <QTimer>

#include <Kanoop/commonexception.h>

const HttpOperation::RequestMethodToStringMap HttpOperation::_RequestMethodToStringMap;

const QList<QSslError::SslError> HttpOperation::SelfSignedCertificateErrors = {
    QSslError::SelfSignedCertificateInChain,
    QSslError::SelfSignedCertificate,
};

HttpOperation::HttpOperation(const QString& url, RequestMethod method) :
    AbstractThreadClass("http-op"),
    _url(url), _method(method)
{
    connect(this, &HttpOperation::abortRequested, this, &HttpOperation::abortReply);
}

HttpOperation::~HttpOperation()
{
}

void HttpOperation::abortOperation()
{
    logText(LVL_DEBUG, QString("TEARDOWN %1: abortOperation enter — crossThread=%2 (caller=0x%3)")
            .arg(objectName())
            .arg(QThread::currentThread() != thread())
            .arg((quintptr)QThread::currentThread(), 0, 16));
    if(QThread::currentThread() != thread()) {
        // Never touch _reply from a foreign thread — the worker deletes and
        // nulls it in threadFinished(). Queue the abort onto our own thread;
        // if the event loop is already gone the event is simply dropped and
        // waitForCompletion() synchronizes with the exit that made it moot.
        emit abortRequested();
        if(waitForCompletion(TimeSpan::fromSeconds(3)) == false) {
            logText(LVL_ERROR, QString("%1: Thread never completed after abort").arg(objectName()));
        }
    }
    else {
        abortReply();
    }
    logText(LVL_DEBUG, QString("TEARDOWN %1: abortOperation exit").arg(objectName()));
}

void HttpOperation::abortReply()
{
    if(_reply != nullptr) {
        _reply->abort();
        _reply = nullptr;
    }
    else {
        logText(LVL_DEBUG, QString("%1: Abort requested with no operation in progress").arg(objectName()));
    }
}

bool HttpOperation::isSelfSignedCertificateErrorIgnored() const
{
    return _ignoreSslErrors.contains(SelfSignedCertificateErrors.first());
}

void HttpOperation::ignoreSelfSignedCertificate()
{
    if(_ignoreSslErrors.contains(SelfSignedCertificateErrors.first()) == false) {
        _ignoreSslErrors.append(SelfSignedCertificateErrors);
    }
}

QNetworkAccessManager* HttpOperation::networkAccessManager()
{
    if(_networkAccessManager == nullptr) {
        _networkAccessManager = new QNetworkAccessManager(this);
        if(_networkProxy.type() != QNetworkProxy::DefaultProxy) {
            _networkAccessManager->setProxy(_networkProxy);
        }
    }
    return _networkAccessManager;
}

void HttpOperation::setReply(QNetworkReply* reply)
{
    if(_reply != nullptr) {
        logText(LVL_ERROR, QString("%1: Set reply while one was already set").arg(objectName()));
    }
    _reply = reply;

    connect(_reply, &QNetworkReply::sslErrors, this, &HttpOperation::onSslErrors);
    connect(_reply, &QNetworkReply::finished, this, &HttpOperation::onReplyFinished);
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

bool HttpOperation::waitForCompletion(const TimeSpan& timeout)
{
    if(timeout != TimeSpan::zero() && _transferTimeout >= timeout) {
        logText(LVL_WARNING, QString("Transfer timeout (%1ms) >= operation timeout (%2ms) — thread may outlive caller")
            .arg(_transferTimeout.totalMilliseconds()).arg(timeout.totalMilliseconds()));
    }
    return AbstractThreadClass::waitForCompletion(timeout);
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
    // Clean up network objects on the correct thread (they were created
    // in threadStarted/execute on this worker thread)
    if(_reply != nullptr) {
        disconnect(_reply, nullptr, this, nullptr);
        _reply->abort();
        delete _reply;
        _reply = nullptr;
    }
    if(_networkAccessManager != nullptr) {
        delete _networkAccessManager;
        _networkAccessManager = nullptr;
    }
    emit operationComplete();
}

void HttpOperation::onReplyFinished()
{
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);

    if(_reply != reply) {
        logText(LVL_ERROR, QString("%1: replies are not equal when finished").arg(objectName()));
    }

    postReplyHook(reply);

    for(const QNetworkReply::RawHeaderPair& pair : reply->rawHeaderPairs()) {
        QString name = pair.first;
        QByteArray payload = pair.second;
        if(QString::compare(name, "set-cookie", Qt::CaseInsensitive) == 0) {
            _responseCookies.append(QNetworkCookie::parseCookies(payload));
        }
    }

    _duration = TimeSpan::fromMilliseconds(_operationStartTime.msecsTo(QDateTime::currentDateTimeUtc()));

    _networkError = reply->error();
    bool success = _networkError == QNetworkReply::NoError;
    QString message = success ? "Operation Successful" : reply->errorString();
    _statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    _reasonPhrase = HttpStatus::reasonPhrase(_statusCode);
    _responseBody = reply->readAll();

    finishAndStop(success, message);
}

void HttpOperation::onSslErrors(const QList<QSslError>& errors)
{
    bool ignore = errors.count() > 0;
    QList<QSslError> expectedErrors;
    for(const QSslError& error : errors) {
        if(_ignoreSslErrors.contains(error.error())) {
            expectedErrors.append(error);
        }
        else {
            ignore = false;
            break;
        }
    }

    if(ignore == true) {
        _reply->ignoreSslErrors(expectedErrors);
    }
}

void HttpOperation::onReplyError(QNetworkReply::NetworkError error)
{
    _networkError = error;
}
