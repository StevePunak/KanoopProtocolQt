#ifndef KANOOPPROTOCOL_H
#define KANOOPPROTOCOL_H
#include <Qt>

#ifdef KANOOP_PROTOCOL_LIBRARY
#define LIBKANOOPPROTOCOL_EXPORT Q_DECL_EXPORT
#else
#define LIBKANOOPPROTOCOL_EXPORT Q_DECL_IMPORT
#endif

#endif // KANOOPPROTOCOL_H
