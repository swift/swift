/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtSwiftUtil_H
#define SWIFT_QtSwiftUtil_H

#define P2QSTRING(a) QString::fromUtf8(a.getUTF8Data())
#define Q2PSTRING(a) Swift::String(a.toUtf8())

#endif
