/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtSwiftUtil_H
#define SWIFT_QtSwiftUtil_H

#define P2QSTRING(a) QString::fromUtf8(a.c_str())
#define Q2PSTRING(a) std::string(a.toUtf8())

#define B2QDATE(a) QDateTime::fromTime_t((a - boost::posix_time::from_time_t(0)).total_seconds())

#endif
