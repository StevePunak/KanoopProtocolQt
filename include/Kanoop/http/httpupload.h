#ifndef HTTPUPLOAD_H
#define HTTPUPLOAD_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

class HttpUpload : public HttpOperation
{
    Q_OBJECT
public:
    HttpUpload(const QString& url, const QString& filename = QString()) :
        HttpOperation(url),
        _filename(filename) {}

    QString filename() const { return _filename; }
    void setFilename(const QString& value) { _filename = value; }

protected:
    virtual void execute() override;

private:
    QString _filename;

signals:
    void uploadProgress(uint64_t sent, uint64_t total);
};

#endif // HTTPUPLOAD_H
