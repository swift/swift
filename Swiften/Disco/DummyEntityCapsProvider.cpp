/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Disco/DummyEntityCapsProvider.h>

#include <iostream>

namespace Swift {

DiscoInfo::ref DummyEntityCapsProvider::getCaps(const JID& jid) const {
	std::map<JID, DiscoInfo::ref>::const_iterator i = caps.find(jid);
	if (i != caps.end()) {
		return i->second;
	}
	return DiscoInfo::ref();
}

}
