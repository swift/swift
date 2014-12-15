/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <string>

namespace Swift {
	class SWIFTEN_API NickManager {
		public:
			virtual ~NickManager();

			virtual std::string getOwnNick() const = 0;
			virtual void setOwnNick(const std::string& nick) = 0;

			boost::signal<void (const std::string&)> onOwnNickChanged;
	};
}
