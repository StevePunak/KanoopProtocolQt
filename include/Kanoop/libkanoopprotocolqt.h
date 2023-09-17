/******************************************************************************************
**
** libtessgui.h
**
** Moved from my Tesseract Engineering repo to open-source
**
** Author:  Stephen Punak
** Created: Mon Oct 9 11:13:45 2023
**
******************************************************************************************/
#ifndef LIBTESSGUI_H
#define LIBTESSGUI_H

#include <QtCore/qglobal.h>

#if defined(KANOOP_PROTOCOLQT_LIBRARY)
#  define LIBKANOOPPROTOCOLQT_EXPORT Q_DECL_EXPORT
#else
#  define LIBKANOOPPROTOCOLQT_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBTESSGUI_H
