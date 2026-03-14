#ifndef HTTPPOST_H
#define HTTPPOST_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

/** @brief HTTP POST operation executed asynchronously on a dedicated thread. */
class LIBKANOOPPROTOCOL_EXPORT HttpPost : public HttpOperation
{
    Q_OBJECT
public:
    /** @brief Construct an HTTP POST operation with an optional raw body.
     * @param url The target URL.
     * @param postBody The raw POST body data. */
    HttpPost(const QString& url, const QByteArray& postBody = QByteArray()) :
        HttpOperation(url, Post),
        _postBody(postBody) {}

    /** @brief Construct an HTTP POST operation with a JSON-serializable body.
     * @param url The target URL.
     * @param postBody The object to serialize to JSON for the POST body. */
    HttpPost(const QString& url, const ISerializableToJson& postBody) :
        HttpOperation(url, Post),
        _postBody(postBody.serializeToJson()), _isJson(true) {}

    /** @brief Return the request body data.
     * @return The POST body as a byte array. */
    QByteArray requestBody() const { return _postBody; }

protected:
    /** @brief Execute the HTTP POST request. */
    virtual void execute() override;

    /** @brief Hook called before the POST request is sent. */
    virtual void prePostHook() {}

    /** @brief Hook called after the POST reply is received. */
    virtual void postPostHook() {}

private:
    QByteArray _postBody;
    bool _isJson = false;
};

#endif // HTTPPOST_H
