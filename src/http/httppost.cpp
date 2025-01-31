#include "http/httppost.h"

void HttpPost::execute()
{
    QNetworkRequest request(url());

    appendHeadersToRequest(&request);

    configureSsl(&request);

    prePostHook();

    setReply(networkAccessManager()->post(request, _postBody));

    postPostHook();
}
