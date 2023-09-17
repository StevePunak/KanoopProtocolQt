#include "http/httppost.h"

void HttpPost::execute()
{
    QNetworkRequest request(url());

    appendHeadersToRequest(&request);

    prePostHook();

    setReply(networkAccessManager()->post(request, _postBody));

    postPostHook();
}
