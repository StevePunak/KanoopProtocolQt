#ifndef HTTPDELETE_H
#define HTTPDELETE_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

class LIBKANOOPPROTOCOL_EXPORT HttpDelete : public HttpOperation
{
    Q_OBJECT
public:
    HttpDelete(const QString& url, const QByteArray& postBody = QByteArray()) :
        HttpOperation(url, Delete),
        _postBody(postBody) {}

    HttpDelete(const QString& url, const ISerializableToJson& postBody) :
        HttpOperation(url, Delete),
        _postBody(postBody.serializeToJson()), _isJson(true) {}

protected:
    virtual void execute() override;

    virtual void preDeleteHook() {}
    virtual void postDeleteHook() {}

private:
    QByteArray _postBody;
    bool _isJson = false;
};

#endif // HTTPDELETE_H
