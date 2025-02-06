#include "http/httpupload.h"

#include <QFileInfo>
#include <QHttpPart>

#include <Kanoop/commonexception.h>

void HttpUpload::execute()
{
    try
    {
        QNetworkRequest request(url());

        configureSsl(&request);

        QFileInfo fileInfo(_filename);
        if(fileInfo.exists() == false) {
            throw CommonException(QString("File '%1' not found for upload").arg(_filename));
        }

        _file = new QFile(_filename, this);

        QHttpPart dispositionPart;
        dispositionPart.setHeader(
                    QNetworkRequest::ContentDispositionHeader,
                    QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileInfo.fileName()));

        _multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

        if(_file->open(QIODevice::ReadOnly) == false) {
            throw CommonException(QString("Failed to open file '%1'").arg(_filename));
        }

        dispositionPart.setBodyDevice(_file);

        _multipart->append(dispositionPart);

        // SSL TODO

        QNetworkReply* reply = networkAccessManager()->post(request, _multipart);
        connect(reply, &QNetworkReply::uploadProgress, this, &HttpUpload::uploadProgress);
        setReply(reply);
    }
    catch (CommonException& e)
    {
        Log::logText(LVL_ERROR, QString("UPLOAD FAILED: %1").arg(e.message()));
    }
}
