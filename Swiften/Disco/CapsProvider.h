/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/CapsInfo.h>

namespace Swift {
	

	class CapsProvider { 
		public:
			virtual ~CapsProvider() {}

			virtual DiscoInfo::ref getCaps(const std::string&) const = 0;

			boost::signal<void (const std::string&)> onCapsAvailable;
	};
}
