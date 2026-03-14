#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H
#include <QObject>

#include <Kanoop/timespan.h>
#include <Kanoop/utility/loggingbaseclass.h>
#include <Kanoop/mqtt/mqttparameters.h>
#include <Kanoop/kanoopprotocol.h>

/** @brief MQTT client wrapper providing connection management, auto-reconnect, and SSL support. */
class LIBKANOOPPROTOCOL_EXPORT MqttClient : public QObject,
                                            public LoggingBaseClass
{
    Q_OBJECT
public:
    /** @brief Construct an MqttClient with default parameters.
     * @param parent The parent QObject. */
    MqttClient(QObject* parent = nullptr);
    /** @brief Construct an MqttClient with the given MQTT parameters.
     * @param parameters The connection parameters.
     * @param parent The parent QObject. */
    MqttClient(const MqttParameters& parameters, QObject* parent = nullptr);
    /** @brief Destroy the MqttClient and release resources. */
    virtual ~MqttClient();

    /** @brief Start the MQTT client and optionally connect to the broker. */
    virtual void start();
    /** @brief Stop the MQTT client and disconnect from the broker. */
    virtual void stop();

    /** @brief Get the current MQTT connection parameters.
     * @return The connection parameters. */
    MqttParameters parameters() const { return _parameters; }
    /** @brief Set the MQTT connection parameters.
     * @param value The connection parameters. */
    void setParameters(const MqttParameters& value) { _parameters = value; }

    /** @brief Get whether the client connects automatically when started.
     * @return True if auto-connect at start is enabled. */
    bool connectAtStart() const { return _connectAtStart; }
    /** @brief Set whether the client connects automatically when started.
     * @param value True to enable auto-connect at start. */
    void setConnectAtStart(bool value) { _connectAtStart = value; }

    /** @brief Get whether automatic reconnection is enabled.
     * @return True if auto-reconnect is enabled. */
    bool autoReconnect() const { return _autoReconnect; }
    /** @brief Set whether automatic reconnection is enabled.
     * @param value True to enable auto-reconnect. */
    void setAutoReconnect(bool value) { _autoReconnect = value; }

    /** @brief Get the delay between automatic reconnection attempts.
     * @return The reconnect interval. */
    TimeSpan autoReconnectTime() const { return _autoReconnectTime; }
    /** @brief Set the delay between automatic reconnection attempts.
     * @param value The reconnect interval. */
    void setAutoReconnectTime(const TimeSpan& value) { _autoReconnectTime = value; }

    /** @brief Get the MQTT client identifier.
     * @return The client ID string. */
    QString clientId() const { return _clientId; }
    /** @brief Set the MQTT client identifier.
     * @param value The client ID string. */
    void setClientId(const QString& value) { _clientId = value; }

    /** @brief Check whether the client is currently connected to the broker.
     * @return True if connected. */
    bool isConnected() const { return _client != nullptr && _connected; }

    /** @brief Subscribe to an MQTT topic.
     * @param topic The topic filter string.
     * @param qos The quality-of-service level (0, 1, or 2).
     * @return A pointer to the resulting QMqttSubscription, or nullptr on failure. */
    QMqttSubscription* subscribe(const QString& topic, uint8_t qos = 0);
    /** @brief Publish a message to an MQTT topic.
     * @param topic The topic to publish to.
     * @param payload The message payload.
     * @param qos The quality-of-service level (0, 1, or 2).
     * @param retain True to set the retain flag on the message.
     * @return The packet identifier of the published message, or -1 on failure. */
    int publish(const QString& topic, const QByteArray& payload, uint8_t qos = 0, bool retain = false);

protected:
    /** @brief Get the underlying QMqttClient instance.
     * @return A pointer to the QMqttClient. */
    QMqttClient* client() const { return _client; }

    /** @brief Called when the client successfully connects to the broker. */
    virtual void connected() {}
    /** @brief Called when the client disconnects from the broker. */
    virtual void disconnected() {}
    /** @brief Called when a client error occurs.
     * @param error The error code. */
    virtual void error(QMqttClient::ClientError error) { Q_UNUSED(error) }

private:
    MqttParameters _parameters;
    bool _connectAtStart = true;
    bool _autoReconnect = true;
    TimeSpan _autoReconnectTime = TimeSpan::fromSeconds(1);
    QString _clientId;
    bool _connected = false;

    QSslConfiguration _sslConfig;
    QMqttClient* _client = nullptr;

signals:
    /** @brief Emitted when the client connects to the broker. */
    void clientConnected();
    /** @brief Emitted when the client disconnects from the broker. */
    void clientDisconnected();
    /** @brief Emitted when a client error occurs.
     * @param error The error code. */
    void clientError(QMqttClient::ClientError error);

private slots:
    void doConnect();
    void onClientConnected();
    void onDisconnected();
    void onErrorChanged(QMqttClient::ClientError error);
};

#endif // MQTTCLIENT_H
