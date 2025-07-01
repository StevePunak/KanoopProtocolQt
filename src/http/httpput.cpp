#include "http/httpput.h"

void HttpPut::execute()
{
    QNetworkRequest request(url());

    appendHeadersToRequest(&request);

    configureSsl(&request);

    prePutHook();

    request.setTransferTimeout(transferTimeout().totalMilliseconds());

    setReply(networkAccessManager()->put(request, _putBody));

    postPutHook();
}
