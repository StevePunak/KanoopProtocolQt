#ifndef MQTTPARAMETERS_H
#define MQTTPARAMETERS_H
#include <QString>
#include <QMqttClient>
#include <QSslKey>
#include <Kanoop/kanoopprotocol.h>

/** @brief Encapsulates connection parameters for an MQTT broker. */
class LIBKANOOPPROTOCOL_EXPORT MqttParameters
{
public:
    /** @brief Construct default MQTT parameters. */
    MqttParameters() {}
    /** @brief Construct MQTT parameters with the given host and port.
     * @param host The broker hostname or IP address.
     * @param port The broker port number (default 1883). */
    MqttParameters(const QString& host, int port = 1883) :
        _host(host), _port(port) {}

    /** @brief Get the broker hostname.
     * @return The broker hostname or IP address. */
    QString host() const { return _host; }
    /** @brief Set the broker hostname.
     * @param value The broker hostname or IP address. */
    void setHost(const QString& value) { _host = value; }

    /** @brief Get the broker port number.
     * @return The port number. */
    int port() const { return _port; }
    /** @brief Set the broker port number.
     * @param value The port number. */
    void setPort(int value) { _port = value; }

    /** @brief Get the authentication username.
     * @return The username string. */
    QString username() const { return _username; }
    /** @brief Set the authentication username.
     * @param value The username string. */
    void setUsername(const QString& value) { _username = value; }

    /** @brief Get the authentication password.
     * @return The password string. */
    QString password() const { return _password; }
    /** @brief Set the authentication password.
     * @param value The password string. */
    void setPassword(const QString& value) { _password = value; }

    /** @brief Get the MQTT client identifier.
     * @return The client ID string. */
    QString clientId() const { return _clientId; }
    /** @brief Set the MQTT client identifier.
     * @param value The client ID string. */
    void setClientId(const QString& value) { _clientId = value; }

    /** @brief Get the MQTT protocol version.
     * @return The protocol version enum value. */
    QMqttClient::ProtocolVersion protocolVersion() const { return _protocolVersion; }
    /** @brief Set the MQTT protocol version.
     * @param value The protocol version enum value. */
    void setProtocolVersion(QMqttClient::ProtocolVersion value) { _protocolVersion = value; }

    /** @brief Get whether SSL/TLS is enabled.
     * @return True if SSL is enabled. */
    bool useSsl() const { return _useSsl; }
    /** @brief Set whether SSL/TLS is enabled.
     * @param value True to enable SSL. */
    void setUseSsl(bool value) { _useSsl = value; }

    /** @brief Get whether peer certificate verification is enabled.
     * @return True if peer verification is enabled. */
    bool verifyPeer() const { return _verifyPeer; }
    /** @brief Set whether peer certificate verification is enabled.
     * @param value True to enable peer verification. */
    void setVerifyPeer(bool value) { _verifyPeer = value; }

    /** @brief Get the client SSL certificate.
     * @return The client certificate. */
    QSslCertificate clientCertificate() const { return _clientCertificate; }
    /** @brief Set the client SSL certificate.
     * @param value The client certificate. */
    void setClientCertificate(const QSslCertificate& value) { _clientCertificate = value; }
    /** @brief Set the client SSL certificate from a PEM file.
     * @param filename Path to the certificate file. */
    void setClientCertificate(const QString& filename);

    /** @brief Get the CA certificate used to verify the broker.
     * @return The CA certificate. */
    QSslCertificate caCertificate() const { return _caCertificate; }
    /** @brief Set the CA certificate used to verify the broker.
     * @param value The CA certificate. */
    void setCaCertificate(const QSslCertificate& value) { _caCertificate = value; }
    /** @brief Set the CA certificate from a PEM file.
     * @param filename Path to the CA certificate file. */
    void setCaCertificate(const QString& filename);

    /** @brief Get the client private key for SSL authentication.
     * @return The private key. */
    QSslKey privateKey() const { return _privateKey; }
    /** @brief Set the client private key for SSL authentication.
     * @param value The private key. */
    void setPrivateKey(const QSslKey& value) { _privateKey = value; }
    /** @brief Set the client private key from a PEM file.
     * @param filename Path to the private key file. */
    void setPrivateKey(const QString& filename);

private:
    QString _host;
    int _port = 1883;
    QString _username;
    QString _password;
    QString _clientId;
    QMqttClient::ProtocolVersion _protocolVersion = QMqttClient::MQTT_5_0;
    bool _useSsl = false;
    bool _verifyPeer = false;

    QSslCertificate _clientCertificate;
    QSslCertificate _caCertificate;
    QSslKey _privateKey;
};

#endif // MQTTPARAMETERS_H
