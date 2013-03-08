/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/BlockListImpl.h>

#include <Swiften/Base/foreach.h>

#include <algorithm>

using namespace Swift;

BlockListImpl::BlockListImpl() : state(Init) {

}

void BlockListImpl::setItems(const std::vector<JID>& items) {
	foreach (const JID& jid, this->items) {
		if (std::find(items.begin(), items.end(), jid) != items.end()) {
			onItemRemoved(jid);
		}
	}

	foreach (const JID& jid, items) {
		if (std::find(this->items.begin(), this->items.end(), jid) != this->items.end()) {
			onItemAdded(jid);
		}
	}
	this->items = items;
}

void BlockListImpl::addItem(const JID& item) {
	if (std::find(items.begin(), items.end(), item) == items.end()) {
		items.push_back(item);
		onItemAdded(item);
	}
}

void BlockListImpl::removeItem(const JID& item) {
	size_t oldSize = items.size();
	items.erase(std::remove(items.begin(), items.end(), item), items.end());
	if (items.size() != oldSize) {
		onItemRemoved(item);
	}
}

void BlockListImpl::setState(State state) {
	if (this->state != state) {
		this->state = state;
		onStateChanged();
	}
}

void BlockListImpl::addItems(const std::vector<JID>& items) {
	foreach (const JID& item, items) {
		addItem(item);
	}
}

void BlockListImpl::removeItems(const std::vector<JID>& items) {
	std::vector<JID> itemsToRemove = items;
	foreach (const JID& item, itemsToRemove) {
		removeItem(item);
	}
}

void BlockListImpl::removeAllItems() {
	removeItems(items);
}

