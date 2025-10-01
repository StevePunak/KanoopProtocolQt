#include "http/httpput.h"

void HttpPut::execute()
{
    QUrl url = HttpOperation::url();
    setUrl(url.toString(QUrl::PrettyDecoded));

    QNetworkRequest request(url);

    appendHeadersToRequest(&request);

    configureSsl(&request);

    prePutHook();

    request.setTransferTimeout(transferTimeout().totalMilliseconds());

    setReply(networkAccessManager()->put(request, _putBody));

    postPutHook();
}
