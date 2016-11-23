/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Roster/ContactRosterItem.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/Idle.h>
#include <Swiften/Elements/Presence.h>

#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>

namespace Swift {


ContactRosterItem::ContactRosterItem(const JID& jid, const JID& displayJID, const std::string& name, GroupRosterItem* parent)
: RosterItem(name, parent), jid_(jid), displayJID_(displayJID.toBare()), mucRole_(MUCOccupant::NoRole), mucAffiliation_(MUCOccupant::NoAffiliation), blockState_(BlockingNotSupported)
{
}

ContactRosterItem::~ContactRosterItem() {
}

StatusShow::Type ContactRosterItem::getStatusShow() const {
    return presence_ ? presence_->getShow() : StatusShow::None;
}

StatusShow::Type ContactRosterItem::getSimplifiedStatusShow() const {
    switch (presence_ ? presence_->getShow() : StatusShow::None) {
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
    return presence_ ? presence_->getStatus() : "";
}

std::string ContactRosterItem::getIdleText() const {
    boost::posix_time::ptime idleTime = getIdle();
    if (idleTime.is_not_a_date_time()) {
        return "";
    } else {
        return dateTimeToLocalString(idleTime);
    }
}

boost::posix_time::ptime ContactRosterItem::getIdle() const {
    Idle::ref idle = presence_ ? presence_->getPayload<Idle>() : Idle::ref();
    if (idle) {
        return idle->getSince();
    }
    else {
        return boost::posix_time::not_a_date_time;
    }
}

std::string ContactRosterItem::getOfflineSinceText() const {
    boost::posix_time::ptime offlineSince = getOfflineSince();
    if (!offlineSince.is_not_a_date_time()) {
        return dateTimeToLocalString(offlineSince);
    }
    return "";
}

boost::posix_time::ptime ContactRosterItem::getOfflineSince() const {
    boost::posix_time::ptime offlineSince = boost::posix_time::not_a_date_time;
    if (presence_ && presence_->getType() == Presence::Unavailable) {
        boost::optional<boost::posix_time::ptime> delay = presence_->getTimestamp();
        if (delay) {
            offlineSince = delay.get();
        }
    }
    return offlineSince;
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


typedef std::pair<std::string, std::shared_ptr<Presence> > StringPresencePair;

void ContactRosterItem::clearPresence() {
    presence_.reset();
    onDataChanged();
}

void ContactRosterItem::applyPresence(std::shared_ptr<Presence> presence) {
    presence_ = presence;
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
