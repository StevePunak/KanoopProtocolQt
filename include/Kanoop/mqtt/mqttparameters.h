#ifndef MQTTPARAMETERS_H
#define MQTTPARAMETERS_H
#include <QString>
#include <QMqttClient>
#include <QSslKey>

class MqttParameters
{
public:
    MqttParameters() {}
    MqttParameters(const QString& host, int port = 1883) :
        _host(host), _port(port) {}

    QString host() const { return _host; }
    void setHost(const QString& value) { _host = value; }

    int port() const { return _port; }
    void setPort(int value) { _port = value; }

    QString username() const { return _username; }
    void setUsername(const QString& value) { _username = value; }

    QString password() const { return _password; }
    void setPassword(const QString& value) { _password = value; }

    QString clientId() const { return _clientId; }
    void setClientId(const QString& value) { _clientId = value; }

    QMqttClient::ProtocolVersion protocolVersion() const { return _protocolVersion; }
    void setProtocolVersion(QMqttClient::ProtocolVersion value) { _protocolVersion = value; }

    bool useSsl() const { return _useSsl; }
    void setUseSsl(bool value) { _useSsl = value; }

    bool verifyPeer() const { return _verifyPeer; }
    void setVerifyPeer(bool value) { _verifyPeer = value; }

    QSslCertificate clientCertificate() const { return _clientCertificate; }
    void setClientCertificate(const QSslCertificate& value) { _clientCertificate = value; }
    void setClientCertificate(const QString& filename);

    QSslCertificate caCertificate() const { return _caCertificate; }
    void setCaCertificate(const QSslCertificate& value) { _caCertificate = value; }
    void setCaCertificate(const QString& filename);

    QSslKey privateKey() const { return _privateKey; }
    void setPrivateKey(const QSslKey& value) { _privateKey = value; }
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
    QSslConfiguration _configuration;

    QSslCertificate _clientCertificate;
    QSslCertificate _caCertificate;
    QSslKey _privateKey;
};

#endif // MQTTPARAMETERS_H
