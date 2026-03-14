#ifndef HTTPHEADERS_H
#define HTTPHEADERS_H

#include <QMap>
#include <QNetworkRequest>
#include <Kanoop/kanoopprotocol.h>

/** @brief Map of custom HTTP header names to their values. */
class LIBKANOOPPROTOCOL_EXPORT HttpCustomHeaders : public QMap<QString, QByteArray>
{
};

/** @brief Map of Qt known HTTP headers to their values. */
class LIBKANOOPPROTOCOL_EXPORT HttpKnownHeaders : public QMap<QNetworkRequest::KnownHeaders, QByteArray>
{
};

#endif // HTTPHEADERS_H
