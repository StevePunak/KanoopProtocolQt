#ifndef HTTPPOST_H
#define HTTPPOST_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

class LIBKANOOPPROTOCOL_EXPORT HttpPost : public HttpOperation
{
    Q_OBJECT
public:
    HttpPost(const QString& url, const QByteArray& postBody) :
        HttpOperation(url, Post),
        _postBody(postBody) {}

    HttpPost(const QString& url, const ISerializableToJson& postBody) :
        HttpOperation(url, Post),
        _postBody(postBody.serializeToJson()), _isJson(true) {}

    QByteArray requestBody() const { return _postBody; }

protected:
    virtual void execute() override;

    virtual void prePostHook() {}
    virtual void postPostHook() {}

private:
    QByteArray _postBody;
    bool _isJson = false;
};

#endif // HTTPPOST_H
