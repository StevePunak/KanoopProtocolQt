#ifndef HTTPHEADERS_H
#define HTTPHEADERS_H

#include <QMap>
#include <QNetworkRequest>


class HttpCustomHeaders : public QMap<QString, QByteArray>
{
};

class HttpKnownHeaders : public QMap<QNetworkRequest::KnownHeaders, QByteArray>
{
};

#endif // HTTPHEADERS_H
