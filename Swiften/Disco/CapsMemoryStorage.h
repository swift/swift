/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/Disco/CapsStorage.h"

namespace Swift {
	class CapsMemoryStorage : public CapsStorage {
		public:
			CapsMemoryStorage() {}

			virtual DiscoInfo::ref getDiscoInfo(const String& hash) const {
				CapsMap::const_iterator i = caps.find(hash);
				if (i != caps.end()) {
					return i->second;
				}
				else {
					return DiscoInfo::ref();
				}
			}

			virtual void setDiscoInfo(const String& hash, DiscoInfo::ref discoInfo) {
				caps[hash] = discoInfo;
			}

		private:
			typedef std::map<String, DiscoInfo::ref> CapsMap;
			CapsMap caps;
	};
}
