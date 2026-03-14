#ifndef HTTPOPERATION_H
#define HTTPOPERATION_H
#include <Kanoop/utility/abstractthreadclass.h>
#include <Kanoop/http/httpheaders.h>

#include <QNetworkCookie>
#include <QNetworkReply>
#include <Kanoop/kanoopprotocol.h>

class QNetworkAccessManager;
/** @brief Base class for HTTP operations executed asynchronously on a dedicated thread. */
class LIBKANOOPPROTOCOL_EXPORT HttpOperation : public AbstractThreadClass
{
    Q_OBJECT
public:
    /** @brief Enumeration of supported HTTP request methods. */
    enum RequestMethod {
        UnknownMethod   = 0x0000,   /**< @brief Unknown or uninitialized request method. */
        Get             = 0x0001,   /**< @brief HTTP GET method. */
        Put             = 0x0002,   /**< @brief HTTP PUT method. */
        Delete          = 0x0004,   /**< @brief HTTP DELETE method. */
        Post            = 0x0008,   /**< @brief HTTP POST method. */
        Head            = 0x0010,   /**< @brief HTTP HEAD method. */
        Patch           = 0x0040,   /**< @brief HTTP PATCH method. */
        MultipartUpload = 0x1000,   /**< @brief Multipart file upload method. */
    };

protected:
    /** @brief Construct an HTTP operation for the given URL and request method.
     * @param url The target URL for the operation.
     * @param method The HTTP request method to use. */
    HttpOperation(const QString& url, RequestMethod method/* = UnknownMethod*/);

public:
    /** @brief Destructor. */
    virtual ~HttpOperation();

    /** @brief Return the target URL of this operation.
     * @return The URL string. */
    QString url() const { return _url; }

    /** @brief Return the known HTTP headers set on this operation.
     * @return The known headers map. */
    HttpKnownHeaders headers() const { return _headers; }

    /** @brief Return the custom HTTP headers set on this operation.
     * @return The custom headers map. */
    HttpCustomHeaders customHeaders() const { return _customHeaders; }

    /** @brief Append a known HTTP header to the request.
     * @param type The known header type.
     * @param value The header value. */
    void appendHeader(QNetworkRequest::KnownHeaders type, const QByteArray& value) { _headers.insert(type, value); }

    /** @brief Append a custom HTTP header to the request.
     * @param headerName The custom header name.
     * @param value The header value. */
    void appendHeader(const QString& headerName, const QByteArray& value) { _customHeaders.insert(headerName, value); }

    /** @brief Abort the running HTTP operation. */
    void abortOperation();

    /** @brief Return the cookies to be sent with the request.
     * @return The list of request cookies. */
    QList<QNetworkCookie> requestCookies() const { return _requestCookies; }

    /** @brief Set the cookies to be sent with the request.
     * @param value The list of cookies. */
    void setRequestCookies(const QList<QNetworkCookie>& value) { _requestCookies = value; }

    /** @brief Return the cookies received in the response.
     * @return The list of response cookies. */
    QList<QNetworkCookie> responseCookies() const { return _responseCookies; }

    /** @brief Return whether peer SSL certificate verification is enabled.
     * @return True if peer verification is enabled. */
    bool isVerifyPeer() const { return _verifyPeer; }

    /** @brief Enable or disable peer SSL certificate verification.
     * @param value True to verify the peer certificate. */
    void setVerifyPeer(bool value) { _verifyPeer = value; }

    /** @brief Return whether self-signed certificate errors are being ignored.
     * @return True if self-signed certificate errors are ignored. */
    bool isSelfSignedCertificateErrorIgnored() const;

    /** @brief Configure the operation to ignore self-signed certificate errors. */
    void ignoreSelfSignedCertificate();

    /** @brief Return the transfer timeout duration.
     * @return The transfer timeout as a TimeSpan. */
    TimeSpan transferTimeout() const { return _transferTimeout; }

    /** @brief Set the transfer timeout duration.
     * @param value The timeout as a TimeSpan. */
    void setTransferTimeout(const TimeSpan& value) { _transferTimeout = value; }

    /** @brief Return the network error from the completed operation.
     * @return The QNetworkReply::NetworkError code. */
    QNetworkReply::NetworkError networkError() const { return _networkError; }

    /** @brief Return the HTTP reason phrase from the response.
     * @return The reason phrase string. */
    QString reasonPhrase() const { return _reasonPhrase; }

    /** @brief Return the response body data.
     * @return The response body as a byte array. */
    QByteArray responseBody() const { return _responseBody; }

    /** @brief Return the HTTP status code from the response.
     * @return The status code integer. */
    int statusCode() const { return _statusCode; }

    /** @brief Return the wall-clock duration of the HTTP operation.
     * @return The duration as a TimeSpan. */
    TimeSpan duration() const { return _duration; }

    /** @brief Return a human-readable string for this operation's request method.
     * @return The request method string. */
    QString getRequestMethodString() const { return _RequestMethodToStringMap.getString(_method); }

    /** @brief Return whether this operation uses HTTPS.
     * @return True if the URL scheme is HTTPS. */
    bool isHttps() const;

    /** @brief Return a human-readable string for the given request method.
     * @param method The request method to convert.
     * @return The request method string. */
    static QString getRequestMethodString(RequestMethod method) { return _RequestMethodToStringMap.getString(method); }

protected:
    /** @brief A simple key-value pair used for HTTP parameters. */
    class KeyValuePair : public QPair<QString, QString>
    {
    public:
        /** @brief Construct an empty key-value pair. */
        KeyValuePair() {}

        /** @brief Construct a key-value pair with the given key and value.
         * @param key The parameter key.
         * @param value The parameter value. */
        KeyValuePair(const QString& key, const QString& value)
        {
            first = key;
            second = value;
        }
    };

    /** @brief Execute the HTTP operation (implemented by subclasses). */
    virtual void execute() = 0;

    /** @brief Hook called after the network reply is received. */
    virtual void postReplyHook(QNetworkReply*) {}

    /** @brief Set the target URL of this operation.
     * @param value The URL string. */
    void setUrl(const QString& value) { _url = value; }

    /** @brief Return the shared network access manager, creating it if necessary.
     * @return A pointer to the QNetworkAccessManager. */
    QNetworkAccessManager* networkAccessManager();

    /** @brief Store the network reply and connect its signals.
     * @param reply The network reply to track. */
    void setReply(QNetworkReply* reply);

    /** @brief Append all configured headers to the given network request.
     * @param request The network request to modify. */
    void appendHeadersToRequest(QNetworkRequest* request);

    /** @brief Configure SSL settings on the given network request.
     * @param request The network request to configure. */
    void configureSsl(QNetworkRequest* request);

    /** @brief Called when the operation thread finishes. */
    virtual void threadFinished() override;

private:
    virtual void threadStarted() override;

    QString _url;
    RequestMethod _method;

    HttpKnownHeaders _headers;
    HttpCustomHeaders _customHeaders;
    QList<QNetworkCookie> _requestCookies;

    int _statusCode = 0;
    QNetworkReply::NetworkError _networkError = QNetworkReply::UnknownServerError;
    QString _reasonPhrase;
    QByteArray _responseBody;
    QList<QNetworkCookie> _responseCookies;

    QList<QSslError::SslError> _ignoreSslErrors;

    bool _verifyPeer = true;

    QDateTime _operationStartTime;
    TimeSpan _transferTimeout = TimeSpan::fromMilliseconds(QNetworkRequest::DefaultTransferTimeoutConstant);
    TimeSpan _duration;

    QNetworkAccessManager* _networkAccessManager = nullptr;
    QNetworkReply* _reply = nullptr;

    class RequestMethodToStringMap : public KANOOP::EnumToStringMap<RequestMethod>
    {
    public:
        RequestMethodToStringMap()
        {
            insert(UnknownMethod,   "UnknownMethod");
            insert(Get,             "GET");
            insert(Put,             "PUT");
            insert(Delete,          "DELETE");
            insert(Post,            "POST");
            insert(Head,            "HEAD");
            insert(Patch,           "PATCH");
            insert(MultipartUpload, "UPLOAD");
        }
    };

    static const RequestMethodToStringMap _RequestMethodToStringMap;
    static const QList<QSslError::SslError> SelfSignedCertificateErrors;

signals:
    /** @brief Emitted when the HTTP operation has completed. */
    void operationComplete();

protected slots:
    /** @brief Handle the network reply finished signal. */
    void onReplyFinished();

    /** @brief Handle SSL errors from the network reply.
     * @param errors The list of SSL errors encountered. */
    void onSslErrors(const QList<QSslError> &errors);

    /** @brief Handle a network error from the reply.
     * @param error The network error code. */
    void onReplyError(QNetworkReply::NetworkError error);
};

#endif // HTTPOPERATION_H
