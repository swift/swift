/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include <string>
#include <Swiften/Base/API.h>
#include <Swiften/Disco/CapsStorage.h>

namespace Swift {
	class SWIFTEN_API CapsMemoryStorage : public CapsStorage {
		public:
			CapsMemoryStorage() {}

			virtual DiscoInfo::ref getDiscoInfo(const std::string& hash) const {
				CapsMap::const_iterator i = caps.find(hash);
				if (i != caps.end()) {
					return i->second;
				}
				else {
					return DiscoInfo::ref();
				}
			}

			virtual void setDiscoInfo(const std::string& hash, DiscoInfo::ref discoInfo) {
				caps[hash] = discoInfo;
			}

		private:
			typedef std::map<std::string, DiscoInfo::ref> CapsMap;
			CapsMap caps;
	};
}
