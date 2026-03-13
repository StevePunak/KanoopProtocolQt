#include "http/httppost.h"

void HttpPost::execute()
{
    QNetworkRequest request(url());

    if(_isJson) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }

    appendHeadersToRequest(&request);

    configureSsl(&request);

    prePostHook();

    request.setTransferTimeout(transferTimeout().totalMilliseconds());

    setReply(networkAccessManager()->post(request, _postBody));

    postPostHook();
}
