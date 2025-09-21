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
    setReply(reply);

    postGetHook(reply);
}


