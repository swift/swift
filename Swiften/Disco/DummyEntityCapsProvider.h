/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <Swiften/Disco/EntityCapsProvider.h>

namespace Swift {
	class DummyEntityCapsProvider : public EntityCapsProvider {
		public:
			DummyEntityCapsProvider() {
			}

			DiscoInfo::ref getCaps(const JID& jid) const;

			std::map<JID, DiscoInfo::ref> caps;
	};
}
