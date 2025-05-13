#ifndef HTTPGET_H
#define HTTPGET_H
#include <Kanoop/kanoopprotocol.h>
#include <Kanoop/http/httpoperation.h>

class LIBKANOOPPROTOCOL_EXPORT HttpGet : public HttpOperation
{
    Q_OBJECT
public:
    HttpGet(const QString& url);

    void addParameter(const QString& key, const QString& value) { _parameters.insert(key, value); }
    void removeParameter(const QString& key) { _parameters.remove(key); }

protected:
    virtual void execute() override;

    virtual void preGetHook(QNetworkRequest*) {}
    virtual void postGetHook(QNetworkReply* reply) { Q_UNUSED(reply) }

private:
    QMap<QString, QString> _parameters;
};

#endif // HTTPGET_H
