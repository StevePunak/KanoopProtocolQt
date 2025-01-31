#include "http/httpdelete.h"

void HttpDelete::execute()
{
    QNetworkRequest request(url());

    appendHeadersToRequest(&request);

    configureSsl(&request);

    preDeleteHook();

    setReply(networkAccessManager()->deleteResource(request));

    postDeleteHook();
}
