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

    /**
     * @brief streamingRead
     * Set to true to return the response piece by piece as available
     * using the `dataAvailable()` signal
     * @return
     */
    bool streamingRead() const { return _streamingRead; }
    void setStreamingRead(bool value) { _streamingRead = value; }

signals:
    void dataAvailable(const QByteArray& data);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

protected:
    virtual void execute() override;

    virtual void preGetHook(QNetworkRequest*) {}
    virtual void postGetHook(QNetworkReply* reply) { Q_UNUSED(reply) }
    virtual void postReplyHook(QNetworkReply*reply) override;

private:
    QList<KeyValuePair> _parameters;
    bool _streamingRead = false;

private slots:
    void onStreamingReadyRead();
};

#endif // HTTPGET_H
