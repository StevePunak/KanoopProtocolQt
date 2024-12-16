/******************************************************************************************
**
** libtessgui.h
**
** Copyright (C) Tesseract Engineering, Inc - All Rights Reserved
**
** This source code is protected under international copyright law.  All rights
** reserved and protected by the copyright holders.
**
** This file is confidential and only available to authorized individuals with the
** permission of the copyright holders.  If you encounter this file and do not have
** permission, please contact the copyright holders and delete this file.
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
