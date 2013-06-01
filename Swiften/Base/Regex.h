/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {

	namespace Regex {
		SWIFTEN_API std::string escape(const std::string& source);
	}

}
