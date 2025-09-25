#include "http/httpget.h"

#include <QNetworkAccessManager>
#include <QUrlQuery>


HttpGet::HttpGet(const QString& url) :
    HttpOperation(url, Get)
{
}

void HttpGet::addParameter(const QString& key, const QString& value)
{
    _parameters.append(KeyValuePair(key, value));
}

void HttpGet::execute()
{
    QUrl url = HttpOperation::url();

    if(_parameters.count() > 0) {
        QUrlQuery query;
        for(const KeyValuePair& kvp : _parameters) {
            query.addQueryItem(kvp.first, kvp.second);
        }
        url.setQuery(query);
        setUrl(url.toString(QUrl::PrettyDecoded));
    }

    QNetworkRequest request(url);

    appendHeadersToRequest(&request);

    configureSsl(&request);

    preGetHook(&request);

    request.setTransferTimeout(transferTimeout().totalMilliseconds());

    QNetworkReply* reply = networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, &HttpGet::downloadProgress);
    setReply(reply);
    if(_streamingRead) {
        connect(reply, &QNetworkReply::readyRead, this, &HttpGet::onStreamingReadyRead);
    }

    postGetHook(reply);
}

void HttpGet::postReplyHook(QNetworkReply* reply)
{
    if(_streamingRead) {
        QByteArray data = reply->readAll();
        emit dataAvailable(data);
    }
}

void HttpGet::onStreamingReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply != nullptr) {
        QByteArray data = reply->readAll();
        emit dataAvailable(data);
    }
}


