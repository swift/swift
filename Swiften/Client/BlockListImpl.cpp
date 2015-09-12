/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Client/BlockListImpl.h>

#include <algorithm>

#include <Swiften/Base/foreach.h>

using namespace Swift;

BlockListImpl::BlockListImpl() : state(Init) {

}

void BlockListImpl::setItems(const std::vector<JID>& newItems) {
	// JIDs which are in the current list but not in the new list, are removed.
	foreach (const JID& jid, items) {
		if (std::find(newItems.begin(), newItems.end(), jid) == newItems.end()) {
			onItemRemoved(jid);
		}
	}

	// JIDs which are in the new list but not in the current list, are added.
	foreach (const JID& jid, newItems) {
		if (std::find(items.begin(), items.end(), jid) == items.end()) {
			onItemAdded(jid);
		}
	}
	items = newItems;
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

