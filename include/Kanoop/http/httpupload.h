#ifndef HTTPUPLOAD_H
#define HTTPUPLOAD_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

/** @brief HTTP multipart file upload operation executed asynchronously on a dedicated thread. */
class LIBKANOOPPROTOCOL_EXPORT HttpUpload : public HttpOperation
{
    Q_OBJECT
public:
    /** @brief Construct an HTTP upload operation for the given URL and optional filename.
     * @param url The target URL.
     * @param filename The path to the file to upload. */
    HttpUpload(const QString& url, const QString& filename = QString()) :
        HttpOperation(url, MultipartUpload),
        _filename(filename) {}

    /** @brief Return the filename to be uploaded.
     * @return The file path string. */
    QString filename() const { return _filename; }

    /** @brief Set the filename to be uploaded.
     * @param value The file path string. */
    void setFilename(const QString& value) { _filename = value; }

    /** @brief Add a form parameter to the multipart upload.
     * @param key The parameter name.
     * @param value The parameter value. */
    void addParameter(const QString& key, const QString& value);

protected:
    /** @brief Execute the multipart file upload. */
    virtual void execute() override;

private:
    QString _filename;
    QList<KeyValuePair> _parameters;

    QHttpMultiPart* _multipart = nullptr;
    QFile* _file = nullptr;

signals:
    /** @brief Emitted to report upload progress.
     * @param sent The number of bytes sent so far.
     * @param total The total number of bytes to send. */
    void uploadProgress(uint64_t sent, uint64_t total);
};

#endif // HTTPUPLOAD_H
