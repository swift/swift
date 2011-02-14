/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Client/NickManager.h>

namespace Swift {
	class VCardManager;

	class DummyNickManager : public NickManager {
		public:
			std::string getOwnNick() const {
				return "";
			}

			void setOwnNick(const std::string&) {
			}
	};
}
