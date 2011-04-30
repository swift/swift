/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include <Swiften/JID/JID.h>
#include <Swiften/VCards/VCardStorage.h>

namespace Swift {
	class VCardMemoryStorage : public VCardStorage {
		public:
			VCardMemoryStorage() {}

			virtual VCard::ref getVCard(const JID& jid) const {
				VCardMap::const_iterator i = vcards.find(jid);
				if (i != vcards.end()) {
					return i->second;
				}
				else {
					return VCard::ref();
				}
			}

			virtual void setVCard(const JID& jid, VCard::ref v) {
				vcards[jid] = v;
			}

		private:
			typedef std::map<JID, VCard::ref> VCardMap;
			VCardMap vcards;
	};
}
