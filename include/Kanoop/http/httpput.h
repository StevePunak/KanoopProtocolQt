#ifndef HTTPPUT_H
#define HTTPPUT_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

/** @brief HTTP PUT operation executed asynchronously on a dedicated thread. */
class LIBKANOOPPROTOCOL_EXPORT HttpPut : public HttpOperation
{
    Q_OBJECT
public:
    /** @brief Construct an HTTP PUT operation with an optional raw body.
     * @param url The target URL.
     * @param putBody The raw PUT body data. */
    HttpPut(const QString& url, const QByteArray& putBody = QByteArray()) :
        HttpOperation(url, Put),
        _putBody(putBody) {}

    /** @brief Construct an HTTP PUT operation with a JSON-serializable body.
     * @param url The target URL.
     * @param putBody The object to serialize to JSON for the PUT body. */
    HttpPut(const QString& url, const ISerializableToJson& putBody) :
        HttpOperation(url, Put),
        _putBody(putBody.serializeToJson()), _isJson(true) {}

    /** @brief Return the PUT request body data.
     * @return The PUT body as a byte array. */
    QByteArray putBody() const { return _putBody; }

protected:
    /** @brief Execute the HTTP PUT request. */
    virtual void execute() override;

    /** @brief Hook called before the PUT request is sent. */
    virtual void prePutHook() {}

    /** @brief Hook called after the PUT reply is received. */
    virtual void postPutHook() {}

private:
    QByteArray _putBody;
    bool _isJson = false;
};


#endif // HTTPPUT_H
