#ifndef HTTPHEADERS_H
#define HTTPHEADERS_H

#include <QMap>
#include <QNetworkRequest>
#include <Kanoop/kanoopprotocol.h>

class LIBKANOOPPROTOCOL_EXPORT HttpCustomHeaders : public QMap<QString, QByteArray>
{
};

class LIBKANOOPPROTOCOL_EXPORT HttpKnownHeaders : public QMap<QNetworkRequest::KnownHeaders, QByteArray>
{
};

#endif // HTTPHEADERS_H
