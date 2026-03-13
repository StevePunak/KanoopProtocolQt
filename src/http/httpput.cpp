#include "http/httpput.h"

void HttpPut::execute()
{
    QUrl url = HttpOperation::url();
    setUrl(url.toString(QUrl::PrettyDecoded));

    QNetworkRequest request(url);

    if(_isJson) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }

    appendHeadersToRequest(&request);

    configureSsl(&request);

    prePutHook();

    request.setTransferTimeout(transferTimeout().totalMilliseconds());

    setReply(networkAccessManager()->put(request, _putBody));

    postPutHook();
}
