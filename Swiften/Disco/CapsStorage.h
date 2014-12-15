/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API CapsStorage {
		public:
			virtual ~CapsStorage();

			virtual DiscoInfo::ref getDiscoInfo(const std::string&) const = 0;
			virtual void setDiscoInfo(const std::string&, DiscoInfo::ref) = 0;
	};
}
