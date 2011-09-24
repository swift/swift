/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"

#include <Swiften/Base/foreach.h>

namespace Swift {


ContactRosterItem::ContactRosterItem(const JID& jid, const JID& displayJID, const std::string& name, GroupRosterItem* parent) : RosterItem(name, parent), jid_(jid), displayJID_(displayJID) {
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
	return StatusShow::None;
}

std::string ContactRosterItem::getStatusText() const {
	return shownPresence_ ? shownPresence_->getStatus() : "";
}

void ContactRosterItem::setAvatarPath(const std::string& path) {
	avatarPath_ = path;
	onDataChanged();
}
const std::string& ContactRosterItem::getAvatarPath() const {
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


typedef std::pair<std::string, boost::shared_ptr<Presence> > StringPresencePair;

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
	onDataChanged();
}

void ContactRosterItem::applyPresence(const std::string& resource, boost::shared_ptr<Presence> presence) {
	if (offlinePresence_) {
		offlinePresence_ = boost::shared_ptr<Presence>();
	}
	if (presence->getType() == Presence::Unavailable) {
		if (resource.empty()) {
			/* Unavailable from the bare JID means all resources are offline.*/
			presences_.clear();
		} else {
			if (presences_.find(resource) != presences_.end()) {
				presences_.erase(resource);
			}
		}
		if (presences_.empty()) {
			offlinePresence_ = presence;
		}
	} else {
		presences_[resource] = presence;
	}
	calculateShownPresence();
	onDataChanged();
}

const std::vector<std::string>& ContactRosterItem::getGroups() const {
	return groups_;
}

/** Only used so a contact can know about the groups it's in*/
void ContactRosterItem::addGroup(const std::string& group) {
	groups_.push_back(group);
}
void ContactRosterItem::removeGroup(const std::string& group) {
	groups_.erase(std::remove(groups_.begin(), groups_.end(), group), groups_.end());
}

void ContactRosterItem::setSupportedFeatures(const std::set<Feature>& features) {
	features_ = features;
}

bool ContactRosterItem::supportsFeature(const Feature feature) const {
	return features_.find(feature) != features_.end();
}

}


