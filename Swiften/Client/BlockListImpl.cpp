/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/BlockListImpl.h>

#include <Swiften/Base/foreach.h>

using namespace Swift;

BlockListImpl::BlockListImpl() {

}

void BlockListImpl::setItems(const std::vector<JID>& items) {
	this->items = std::set<JID>(items.begin(), items.end());
}

void BlockListImpl::addItem(const JID& item) {
	if (items.insert(item).second) {
		onItemAdded(item);
	}
}

void BlockListImpl::removeItem(const JID& item) {
	if (items.erase(item)) {
		onItemRemoved(item);
	}
}

void BlockListImpl::setState(State state) {
	if (this->state != state) {
		onStateChanged();
	}
}

void BlockListImpl::addItems(const std::vector<JID>& items) {
	foreach (const JID& item, items) {
		addItem(item);
	}
}

void BlockListImpl::removeItems(const std::vector<JID>& items) {
	foreach (const JID& item, items) {
		removeItem(item);
	}
}

void BlockListImpl::removeAllItems() {
	foreach (const JID& item, items) {
		removeItem(item);
	}
}

