/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <boost/cstdint.hpp>
#include <string>

namespace Swift {

SWIFTEN_API std::string formatSize(const boost::uintmax_t bytes);

}
