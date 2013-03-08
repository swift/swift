/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/BlockList.h>

#include <algorithm>

using namespace Swift;

BlockList::~BlockList() {

}

bool BlockList::isBlocked(const JID& jid) const {
	const std::vector<JID>& items = getItems();
	return (std::find(items.begin(), items.end(), jid.toBare()) != items.end()) ||
			(std::find(items.begin(), items.end(), JID(jid.getDomain())) != items.end()) ||
			(std::find(items.begin(), items.end(), jid) != items.end());
}
