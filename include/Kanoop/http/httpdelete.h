#ifndef HTTPDELETE_H
#define HTTPDELETE_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

/** @brief HTTP DELETE operation executed asynchronously on a dedicated thread. */
class LIBKANOOPPROTOCOL_EXPORT HttpDelete : public HttpOperation
{
    Q_OBJECT
public:
    /** @brief Construct an HTTP DELETE operation with an optional raw body.
     * @param url The target URL.
     * @param postBody The raw body data to include with the DELETE request. */
    HttpDelete(const QString& url, const QByteArray& postBody = QByteArray()) :
        HttpOperation(url, Delete),
        _postBody(postBody) {}

    /** @brief Construct an HTTP DELETE operation with a JSON-serializable body.
     * @param url The target URL.
     * @param postBody The object to serialize to JSON for the DELETE body. */
    HttpDelete(const QString& url, const ISerializableToJson& postBody) :
        HttpOperation(url, Delete),
        _postBody(postBody.serializeToJson()), _isJson(true) {}

protected:
    /** @brief Execute the HTTP DELETE request. */
    virtual void execute() override;

    /** @brief Hook called before the DELETE request is sent. */
    virtual void preDeleteHook() {}

    /** @brief Hook called after the DELETE reply is received. */
    virtual void postDeleteHook() {}

private:
    QByteArray _postBody;
    bool _isJson = false;
};

#endif // HTTPDELETE_H
