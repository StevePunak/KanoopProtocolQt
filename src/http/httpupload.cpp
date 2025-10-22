#include "http/httpupload.h"

#include <QFileInfo>
#include <QHttpPart>
#include <QUrlQuery>

#include <Kanoop/commonexception.h>

void HttpUpload::addParameter(const QString& key, const QString& value)
{
    _parameters.append(KeyValuePair(key, value));
}

void HttpUpload::execute()
{
    try
    {
        QUrl url = HttpOperation::url();

        if(_parameters.count() > 0) {
            QUrlQuery query;
            for(const KeyValuePair& kvp : _parameters) {
                query.addQueryItem(kvp.first, kvp.second);
            }
            url.setQuery(query);
            setUrl(url.toString(QUrl::PrettyDecoded));
        }

        QNetworkRequest request(url);

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
