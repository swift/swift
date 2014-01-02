/*
 * Copyright (c) 2010-2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <boost/cstdint.hpp>
#include <string>

namespace Swift {

SWIFTEN_API std::string formatSize(const boost::uintmax_t bytes);

}
