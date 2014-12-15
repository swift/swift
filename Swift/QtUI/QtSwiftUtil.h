/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#define P2QSTRING(a) QString::fromUtf8(a.c_str())
#define Q2PSTRING(a) std::string(a.toUtf8())

#include <boost/date_time/posix_time/posix_time.hpp>

#define B2QDATE(a) QDateTime::fromTime_t((a - boost::posix_time::from_time_t(0)).total_seconds())
