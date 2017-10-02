/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/cstdint.hpp>

#include <Swiften/Base/API.h>

namespace Swift {

SWIFTEN_API std::string formatSize(const boost::uintmax_t bytes);

}
