#include "http/httppatch.h"

void HttpPatch::execute()
{
    QUrl url = HttpOperation::url();
    setUrl(url.toString(QUrl::PrettyDecoded));

    QNetworkRequest request(url);

    if(_isJson) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }

    appendHeadersToRequest(&request);

    configureSsl(&request);

    prePatchHook();

    request.setTransferTimeout(transferTimeout().totalMilliseconds());

    setReply(networkAccessManager()->sendCustomRequest(request, getRequestMethodString().toLatin1(), _patchBody));

    postPatchHook();
}

#include "Kanoop/http/moc_httppatch.cpp"
