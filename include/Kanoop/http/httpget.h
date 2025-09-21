#ifndef HTTPGET_H
#define HTTPGET_H
#include <Kanoop/kanoopprotocol.h>
#include <Kanoop/http/httpoperation.h>

class LIBKANOOPPROTOCOL_EXPORT HttpGet : public HttpOperation
{
    Q_OBJECT
public:
    HttpGet(const QString& url);

    void addParameter(const QString& key, const QString& value);

protected:
    virtual void execute() override;

    virtual void preGetHook(QNetworkRequest*) {}
    virtual void postGetHook(QNetworkReply* reply) { Q_UNUSED(reply) }

private:
    class KeyValuePair : public QPair<QString, QString>
    {
    public:
        KeyValuePair() {}
        KeyValuePair(const QString& key, const QString& value)
        {
            first = key;
            second = value;
        }
    };
    QList<KeyValuePair> _parameters;
};

#endif // HTTPGET_H
