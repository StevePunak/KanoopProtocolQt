#include "http/httppost.h"

void HttpPost::execute()
{
    QNetworkRequest request(url());

    appendHeadersToRequest(&request);

    configureSsl(&request);

    prePostHook();

    request.setTransferTimeout(transferTimeout().totalMilliseconds());

    setReply(networkAccessManager()->post(request, _postBody));

    postPostHook();
}
