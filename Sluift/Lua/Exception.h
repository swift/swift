/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <stdexcept>

namespace Swift {
	namespace Lua {
		class Exception : public std::runtime_error {
			public:
				Exception(const std::string& what);
				virtual ~Exception() throw();
		};
	}
}

