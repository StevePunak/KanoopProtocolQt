#include "http/httpget.h"

#include <QNetworkAccessManager>
#include <QUrlQuery>


HttpGet::HttpGet(const QString& url) :
    HttpOperation(url, Get)
{
}


void HttpGet::execute()
{
    QUrl url = HttpOperation::url();

    if(_parameters.count() > 0) {
        QUrlQuery query;
        for(auto it = _parameters.constBegin();it !=_parameters.constEnd();it++) {
            query.addQueryItem(it.key(), it.value());
        }
        url.setQuery(query);
        setUrl(url.toString(QUrl::PrettyDecoded));
    }

    QNetworkRequest request(url);

    appendHeadersToRequest(&request);

    configureSsl(&request);

    preGetHook(&request);

    setReply(networkAccessManager()->get(request));

    postGetHook();
}


