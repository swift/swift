/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Roster/ContactRosterItem.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/Idle.h>
#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>

namespace Swift {


ContactRosterItem::ContactRosterItem(const JID& jid, const JID& displayJID, const std::string& name, GroupRosterItem* parent)
: RosterItem(name, parent), jid_(jid), displayJID_(displayJID), mucRole_(MUCOccupant::NoRole), mucAffiliation_(MUCOccupant::NoAffiliation), blockState_(BlockingNotSupported)
{
}

ContactRosterItem::~ContactRosterItem() {
}

StatusShow::Type ContactRosterItem::getStatusShow() const {
	return shownPresence_ ? shownPresence_->getShow() : StatusShow::None;
}

StatusShow::Type ContactRosterItem::getSimplifiedStatusShow() const {
	switch (shownPresence_ ? shownPresence_->getShow() : StatusShow::None) {
		case StatusShow::Online: return StatusShow::Online;
		case StatusShow::Away: return StatusShow::Away;
	 	case StatusShow::XA: return StatusShow::Away;
		case StatusShow::FFC: return StatusShow::Online;
		case StatusShow::DND: return StatusShow::DND;
		case StatusShow::None: return StatusShow::None;
	}
	assert(false);
	return StatusShow::None;
}

std::string ContactRosterItem::getStatusText() const {
	return shownPresence_ ? shownPresence_->getStatus() : "";
}

std::string ContactRosterItem::getIdleText() const {
	Idle::ref idle = shownPresence_ ? shownPresence_->getPayload<Idle>() : Idle::ref();
	if (!idle || idle->getSince().is_not_a_date_time()) {
		return "";
	} else {
		return dateTimeToLocalString(idle->getSince());
	}
}

std::string ContactRosterItem::getOfflineSinceText() const {
	if (offlinePresence_) {
		boost::optional<boost::posix_time::ptime> delay = offlinePresence_->getTimestamp();
		if (offlinePresence_->getType() == Presence::Unavailable && delay) {
			return dateTimeToLocalString(*delay);
		}
	}
	return "";
}

void ContactRosterItem::setAvatarPath(const boost::filesystem::path& path) {
	avatarPath_ = path;
	onDataChanged();
}
const boost::filesystem::path& ContactRosterItem::getAvatarPath() const {
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

MUCOccupant::Role ContactRosterItem::getMUCRole() const
{
	return mucRole_;
}

void ContactRosterItem::setMUCRole(const MUCOccupant::Role& role)
{
	mucRole_ = role;
}

MUCOccupant::Affiliation ContactRosterItem::getMUCAffiliation() const
{
	return mucAffiliation_;
}

void ContactRosterItem::setMUCAffiliation(const MUCOccupant::Affiliation& affiliation)
{
	mucAffiliation_ = affiliation;
}

std::string ContactRosterItem::getMUCAffiliationText() const
{
	std::string affiliationString;
	switch (mucAffiliation_) {
		case MUCOccupant::Owner: affiliationString = QT_TRANSLATE_NOOP("", "Owner"); break;
		case MUCOccupant::Admin: affiliationString = QT_TRANSLATE_NOOP("", "Admin"); break;
		case MUCOccupant::Member: affiliationString = QT_TRANSLATE_NOOP("", "Member"); break;
		case MUCOccupant::Outcast: affiliationString = QT_TRANSLATE_NOOP("", "Outcast"); break;
		case MUCOccupant::NoAffiliation: affiliationString = ""; break;
	}

	return affiliationString;
}

void ContactRosterItem::setSupportedFeatures(const std::set<Feature>& features) {
	features_ = features;
	onDataChanged();
}

bool ContactRosterItem::supportsFeature(const Feature feature) const {
	return features_.find(feature) != features_.end();
}

void ContactRosterItem::setBlockState(BlockState state) {
	blockState_ = state;
	onDataChanged();
}

ContactRosterItem::BlockState ContactRosterItem::blockState() const {
	return blockState_;
}

VCard::ref ContactRosterItem::getVCard() const {
	return vcard_;
}

void ContactRosterItem::setVCard(VCard::ref vcard) {
	vcard_ = vcard;
	onDataChanged();
}

}
