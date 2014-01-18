/*
 * Copyright (c) 2013-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <stdexcept>
#include <Swiften/Base/API.h>

namespace Swift {
	namespace Lua {
		class Exception : public std::runtime_error {
			public:
				Exception(const std::string& what);
				SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Exception)
				virtual ~Exception() SWIFTEN_NOEXCEPT;
		};
	}
}

