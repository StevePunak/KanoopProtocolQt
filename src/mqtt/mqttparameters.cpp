#include "mqtt/mqttparameters.h"

#include <Kanoop/fileutil.h>


void MqttParameters::setClientCertificate(const QString& filename)
{
    QList<QSslCertificate> certs = QSslCertificate::fromPath(filename);
    if(certs.count() > 0) {
        _clientCertificate = certs.at(0);
    }
}

void MqttParameters::setCaCertificate(const QString& filename)
{
    QList<QSslCertificate> certs = QSslCertificate::fromPath(filename);
    if(certs.count() > 0) {
        _caCertificate = certs.at(0);
    }
}

void MqttParameters::setPrivateKey(const QString& filename)
{
    QList<QSsl::KeyAlgorithm> validTypes =
    {
        QSsl::Rsa, QSsl::Ec
    };

    QByteArray data;
    if(FileUtil::readAllBytes(filename, data) == true) {
        for(QSsl::KeyAlgorithm keyType: validTypes) {
            _privateKey = QSslKey(data, keyType);
            if(_privateKey.isNull() == false)
                break;
        }
    }
}
