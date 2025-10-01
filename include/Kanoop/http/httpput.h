#ifndef HTTPPUT_H
#define HTTPPUT_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

class LIBKANOOPPROTOCOL_EXPORT HttpPut : public HttpOperation
{
    Q_OBJECT
public:
    HttpPut(const QString& url, const QByteArray& putBody = QByteArray()) :
        HttpOperation(url, Put),
        _putBody(putBody) {}

    HttpPut(const QString& url, const ISerializableToJson& putBody) :
        HttpOperation(url, Put),
        _putBody(putBody.serializeToJson()), _isJson(true) {}

protected:
    virtual void execute() override;

    virtual void prePutHook() {}
    virtual void postPutHook() {}

private:
    QByteArray _putBody;
    bool _isJson = false;
};


#endif // HTTPPUT_H
