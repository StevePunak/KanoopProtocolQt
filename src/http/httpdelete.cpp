#include "http/httpdelete.h"

void HttpDelete::execute()
{
    QNetworkRequest request(url());

    appendHeadersToRequest(&request);

    preDeleteHook();

    setReply(networkAccessManager()->deleteResource(request));

    postDeleteHook();
}
