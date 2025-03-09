#ifndef HTTPDELETE_H
#define HTTPDELETE_H
#include <Kanoop/http/httpoperation.h>
#include <Kanoop/serialization/iserializabletojson.h>

class HttpDelete : public HttpOperation
{
    Q_OBJECT
public:
    HttpDelete(const QString& url) :
        HttpOperation(url, Delete) {}

protected:
    virtual void execute() override;

    virtual void preDeleteHook() {}
    virtual void postDeleteHook() {}
};

#endif // HTTPDELETE_H
