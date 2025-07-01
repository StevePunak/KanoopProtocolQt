#include "http/httpdelete.h"

void HttpDelete::execute()
{
    QNetworkRequest request(url());

    appendHeadersToRequest(&request);

    configureSsl(&request);

    preDeleteHook();

    request.setTransferTimeout(transferTimeout().totalMilliseconds());
    setReply(networkAccessManager()->deleteResource(request));

    postDeleteHook();
}
