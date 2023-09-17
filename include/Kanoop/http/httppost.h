#ifndef HTTPPOST_H
#define HTTPPOST_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

class HttpPost : public HttpOperation
{
    Q_OBJECT
public:
    HttpPost(const QString& url, const QByteArray& postBody) :
        HttpOperation(url),
        _postBody(postBody) {}

    HttpPost(const QString& url, const ISerializableToJson& postBody) :
        HttpOperation(url),
        _postBody(postBody.serializeToJson()), _isJson(true) {}

protected:
    virtual void execute() override;

    virtual void prePostHook() {}
    virtual void postPostHook() {}

private:
    QByteArray _postBody;
    bool _isJson = false;
};

#endif // HTTPPOST_H
