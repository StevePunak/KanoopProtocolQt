#include "http/httpupload.h"

#include <QFileInfo>
#include <QHttpPart>

#include <Kanoop/commonexception.h>

void HttpUpload::execute()
{
    try
    {
        QNetworkRequest request(url());

        QFileInfo fileInfo(_filename);
        if(fileInfo.exists() == false) {
            throw CommonException(QString("File '%1' not found for upload").arg(_filename));
        }

        QFile file(_filename, this);

        QHttpPart dispositionPart;
        dispositionPart.setHeader(
                    QNetworkRequest::ContentDispositionHeader,
                    QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileInfo.fileName()));

        QHttpMultiPart multipart(QHttpMultiPart::FormDataType, this);

        if(file.open(QIODevice::ReadOnly) == false) {
            throw CommonException(QString("Failed to open file '%1'").arg(_filename));
        }

        dispositionPart.setBodyDevice(&file);

        multipart.append(dispositionPart);

        // SSL TODO

        QNetworkReply* reply = networkAccessManager()->post(request, &multipart);
        connect(reply, &QNetworkReply::uploadProgress, this, &HttpUpload::uploadProgress);
        setReply(reply);
    }
    catch (CommonException& e)
    {
        Log::logText(LVL_ERROR, QString("UPLOAD FAILED: %1").arg(e.message()));
    }
}
