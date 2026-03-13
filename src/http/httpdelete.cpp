#include "http/httpdelete.h"

void HttpDelete::execute()
{
    QNetworkRequest request(url());

    if(_isJson) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }

    appendHeadersToRequest(&request);

    configureSsl(&request);

    preDeleteHook();

    request.setTransferTimeout(transferTimeout().totalMilliseconds());

    setReply(networkAccessManager()->sendCustomRequest(request, "DELETE", _postBody));

    postDeleteHook();
}
