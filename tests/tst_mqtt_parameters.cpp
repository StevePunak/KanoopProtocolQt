#include <QTest>
#include <Kanoop/mqtt/mqttparameters.h>

class TstMqttParameters : public QObject
{
    Q_OBJECT

private slots:
    // ---- Construction ----

    void defaultConstruction()
    {
        MqttParameters p;
        QVERIFY(p.host().isEmpty());
        QCOMPARE(p.port(), 1883);
        QVERIFY(p.username().isEmpty());
        QVERIFY(p.password().isEmpty());
        QVERIFY(p.clientId().isEmpty());
        QCOMPARE(p.protocolVersion(), QMqttClient::MQTT_5_0);
        QVERIFY(!p.useSsl());
        QVERIFY(!p.verifyPeer());
    }

    void hostPortConstruction()
    {
        MqttParameters p("broker.example.com", 8883);
        QCOMPARE(p.host(), QString("broker.example.com"));
        QCOMPARE(p.port(), 8883);
    }

    void hostOnlyConstruction()
    {
        MqttParameters p("localhost");
        QCOMPARE(p.host(), QString("localhost"));
        QCOMPARE(p.port(), 1883); // default
    }

    // ---- Getters / Setters ----

    void setHost()
    {
        MqttParameters p;
        p.setHost("mqtt.test.io");
        QCOMPARE(p.host(), QString("mqtt.test.io"));
    }

    void setPort()
    {
        MqttParameters p;
        p.setPort(9883);
        QCOMPARE(p.port(), 9883);
    }

    void setUsername()
    {
        MqttParameters p;
        p.setUsername("admin");
        QCOMPARE(p.username(), QString("admin"));
    }

    void setPassword()
    {
        MqttParameters p;
        p.setPassword("s3cret");
        QCOMPARE(p.password(), QString("s3cret"));
    }

    void setClientId()
    {
        MqttParameters p;
        p.setClientId("my-client-42");
        QCOMPARE(p.clientId(), QString("my-client-42"));
    }

    void setProtocolVersion()
    {
        MqttParameters p;
        p.setProtocolVersion(QMqttClient::MQTT_3_1_1);
        QCOMPARE(p.protocolVersion(), QMqttClient::MQTT_3_1_1);
    }

    void setUseSsl()
    {
        MqttParameters p;
        QVERIFY(!p.useSsl());
        p.setUseSsl(true);
        QVERIFY(p.useSsl());
    }

    void setVerifyPeer()
    {
        MqttParameters p;
        QVERIFY(!p.verifyPeer());
        p.setVerifyPeer(true);
        QVERIFY(p.verifyPeer());
    }

    // ---- SSL certificate objects ----

    void setClientCertificateObject()
    {
        MqttParameters p;
        QVERIFY(p.clientCertificate().isNull());
        // Setting with a null cert is valid — just verifies the setter works
        QSslCertificate cert;
        p.setClientCertificate(cert);
        QVERIFY(p.clientCertificate().isNull()); // still null, no file loaded
    }

    void setCaCertificateObject()
    {
        MqttParameters p;
        QVERIFY(p.caCertificate().isNull());
        QSslCertificate cert;
        p.setCaCertificate(cert);
        QVERIFY(p.caCertificate().isNull());
    }

    void setPrivateKeyObject()
    {
        MqttParameters p;
        QVERIFY(p.privateKey().isNull());
        QSslKey key;
        p.setPrivateKey(key);
        QVERIFY(p.privateKey().isNull());
    }

    // ---- Copy semantics ----

    void copyConstruction()
    {
        MqttParameters orig("broker.local", 8883);
        orig.setUsername("user");
        orig.setPassword("pass");
        orig.setClientId("client-1");
        orig.setUseSsl(true);
        orig.setVerifyPeer(true);
        orig.setProtocolVersion(QMqttClient::MQTT_3_1_1);

        MqttParameters copy(orig);
        QCOMPARE(copy.host(), orig.host());
        QCOMPARE(copy.port(), orig.port());
        QCOMPARE(copy.username(), orig.username());
        QCOMPARE(copy.password(), orig.password());
        QCOMPARE(copy.clientId(), orig.clientId());
        QCOMPARE(copy.useSsl(), orig.useSsl());
        QCOMPARE(copy.verifyPeer(), orig.verifyPeer());
        QCOMPARE(copy.protocolVersion(), orig.protocolVersion());
    }

    void assignmentOperator()
    {
        MqttParameters orig("broker.local", 8883);
        orig.setUsername("admin");

        MqttParameters assigned;
        assigned = orig;
        QCOMPARE(assigned.host(), orig.host());
        QCOMPARE(assigned.port(), orig.port());
        QCOMPARE(assigned.username(), orig.username());
    }

    // ---- Full configuration round-trip ----

    void fullConfiguration()
    {
        MqttParameters p("iot.example.com", 8883);
        p.setUsername("device-001");
        p.setPassword("token-xyz");
        p.setClientId("pulse-client");
        p.setProtocolVersion(QMqttClient::MQTT_5_0);
        p.setUseSsl(true);
        p.setVerifyPeer(true);

        QCOMPARE(p.host(), QString("iot.example.com"));
        QCOMPARE(p.port(), 8883);
        QCOMPARE(p.username(), QString("device-001"));
        QCOMPARE(p.password(), QString("token-xyz"));
        QCOMPARE(p.clientId(), QString("pulse-client"));
        QCOMPARE(p.protocolVersion(), QMqttClient::MQTT_5_0);
        QVERIFY(p.useSsl());
        QVERIFY(p.verifyPeer());
    }
};

QTEST_MAIN(TstMqttParameters)
#include "tst_mqtt_parameters.moc"
