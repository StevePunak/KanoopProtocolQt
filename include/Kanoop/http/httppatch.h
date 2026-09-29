#ifndef HTTPPATCH_H
#define HTTPPATCH_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

/** @brief HTTP PATCH operation executed asynchronously on a dedicated thread. */
class LIBKANOOPPROTOCOL_EXPORT HttpPatch : public HttpOperation
{
    Q_OBJECT
public:
    /** @brief Construct an HTTP PATCH operation with an optional raw body.
     * @param url The target URL.
     * @param patchBody The raw PATCH body data. */
    HttpPatch(const QString& url, const QByteArray& patchBody = QByteArray()) :
        HttpOperation(url, Patch),
        _patchBody(patchBody) {}

    /** @brief Construct an HTTP PATCH operation with a JSON-serializable body.
     * @param url The target URL.
     * @param patchBody The object to serialize to JSON for the PATCH body. */
    HttpPatch(const QString& url, const ISerializableToJson& patchBody) :
        HttpOperation(url, Patch),
        _patchBody(patchBody.serializeToJson()), _isJson(true) {}

    /** @brief Return the PATCH request body data.
     * @return The PATCH body as a byte array. */
    QByteArray patchBody() const { return _patchBody; }

protected:
    /** @brief Execute the HTTP PATCH request. */
    virtual void execute() override;

    /** @brief Hook called before the PATCH request is sent. */
    virtual void prePatchHook() {}

    /** @brief Hook called after the PATCH reply is received. */
    virtual void postPatchHook() {}

private:
    QByteArray _patchBody;
    bool _isJson = false;
};


#endif // HTTPPATCH_H
