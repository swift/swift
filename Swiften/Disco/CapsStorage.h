/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/DiscoInfo.h>

namespace Swift {
	

	class CapsStorage {
		public:
			virtual ~CapsStorage();

			virtual DiscoInfo::ref getDiscoInfo(const std::string&) const = 0;
			virtual void setDiscoInfo(const std::string&, DiscoInfo::ref) = 0;
	};
}
