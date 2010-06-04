/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Roster/GroupRosterItem.h"

namespace Swift {


ContactRosterItem::ContactRosterItem(const JID& jid, const JID& displayJID, const String& name, GroupRosterItem* parent) : RosterItem(name, parent), jid_(jid), displayJID_(displayJID) {
}

ContactRosterItem::~ContactRosterItem() {
}

StatusShow::Type ContactRosterItem::getStatusShow() const {
	return shownPresence_ ? shownPresence_->getShow() : StatusShow::None;
}

StatusShow::Type ContactRosterItem::getSimplifiedStatusShow() const {
	switch (shownPresence_ ? shownPresence_->getShow() : StatusShow::None) {
		case StatusShow::Online: return StatusShow::Online; break;
		case StatusShow::Away: return StatusShow::Away; break;
	 	case StatusShow::XA: return StatusShow::Away; break;
		case StatusShow::FFC: return StatusShow::Online; break;
		case StatusShow::DND: return StatusShow::DND; break;
		case StatusShow::None: return StatusShow::None; break;
	}
	assert(false);
}

String ContactRosterItem::getStatusText() const {
	return shownPresence_ ? shownPresence_->getStatus() : "";
}

void ContactRosterItem::setAvatarPath(const String& path) {
	avatarPath_ = path;
	onDataChanged();
}
const String& ContactRosterItem::getAvatarPath() const {
	return avatarPath_;
}

const JID& ContactRosterItem::getJID() const {
	return jid_;
}

void ContactRosterItem::setDisplayJID(const JID& jid) {
	displayJID_ = jid;
}

const JID& ContactRosterItem::getDisplayJID() const {
	return displayJID_;
}


typedef std::pair<String, boost::shared_ptr<Presence> > StringPresencePair;

void ContactRosterItem::calculateShownPresence() {
	shownPresence_ = offlinePresence_;
	foreach (StringPresencePair presencePair, presences_) {
		boost::shared_ptr<Presence> presence = presencePair.second;
		if (!shownPresence_ || presence->getPriority() > shownPresence_->getPriority() || presence->getShow() < shownPresence_->getShow()) {
			shownPresence_ = presence;
		}
	}
}

void ContactRosterItem::clearPresence() {
	presences_.clear();
	calculateShownPresence();
}

void ContactRosterItem::applyPresence(const String& resource, boost::shared_ptr<Presence> presence) {
	if (offlinePresence_) {
		offlinePresence_ = boost::shared_ptr<Presence>();
	}
	if (presence->getType() == Presence::Unavailable) {
		if (presences_.find(resource) != presences_.end()) {
			presences_.erase(resource);
		}
		if (presences_.size() > 0) {
			offlinePresence_ = presence;
		}
	} else {
		presences_[resource] = presence;
	}
	calculateShownPresence();
	onDataChanged();
}

}


