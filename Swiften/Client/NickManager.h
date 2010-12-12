/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/String.h>

namespace Swift {
	class NickManager {
		public:
			virtual ~NickManager();

			virtual String getOwnNick() const = 0;
			virtual void setOwnNick(const String& nick) = 0;

			boost::signal<void (const String&)> onOwnNickChanged;
	};
}
