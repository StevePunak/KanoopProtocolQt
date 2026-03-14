#ifndef HTTPGET_H
#define HTTPGET_H
#include <Kanoop/kanoopprotocol.h>
#include <Kanoop/http/httpoperation.h>

/** @brief HTTP GET operation executed asynchronously on a dedicated thread. */
class LIBKANOOPPROTOCOL_EXPORT HttpGet : public HttpOperation
{
    Q_OBJECT
public:
    /** @brief Construct an HTTP GET operation for the given URL.
     * @param url The target URL. */
    HttpGet(const QString& url);

    /** @brief Add a query parameter to the GET request.
     * @param key The parameter name.
     * @param value The parameter value. */
    void addParameter(const QString& key, const QString& value);

    /**
     * @brief streamingRead
     * Set to true to return the response piece by piece as available
     * using the `dataAvailable()` signal
     * @return True if streaming read mode is enabled.
     */
    bool streamingRead() const { return _streamingRead; }

    /** @brief Enable or disable streaming read mode.
     * @param value True to enable streaming reads via the dataAvailable() signal. */
    void setStreamingRead(bool value) { _streamingRead = value; }

signals:
    /** @brief Emitted when a chunk of data is available during a streaming read.
     * @param data The received data chunk. */
    void dataAvailable(const QByteArray& data);

    /** @brief Emitted to report download progress.
     * @param bytesReceived The number of bytes received so far.
     * @param bytesTotal The total number of bytes expected, or -1 if unknown. */
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

protected:
    /** @brief Execute the HTTP GET request. */
    virtual void execute() override;

    /** @brief Hook called before the GET request is sent. */
    virtual void preGetHook(QNetworkRequest*) {}

    /** @brief Hook called after the GET reply is received.
     * @param reply The network reply object. */
    virtual void postGetHook(QNetworkReply* reply) { Q_UNUSED(reply) }

    /** @brief Hook called after the network reply is processed.
     * @param reply The network reply object. */
    virtual void postReplyHook(QNetworkReply*reply) override;

private:
    QList<KeyValuePair> _parameters;
    bool _streamingRead = false;

private slots:
    void onStreamingReadyRead();
};

#endif // HTTPGET_H
