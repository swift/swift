/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Slimber/LinkLocalPresenceManager.h>

#include <boost/bind.hpp>

#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/LinkLocal/LinkLocalServiceBrowser.h>

namespace Swift {

LinkLocalPresenceManager::LinkLocalPresenceManager(LinkLocalServiceBrowser* browser) : browser(browser) {
    browser->onServiceAdded.connect(
            boost::bind(&LinkLocalPresenceManager::handleServiceAdded, this, _1));
    browser->onServiceChanged.connect(
            boost::bind(&LinkLocalPresenceManager::handleServiceChanged, this, _1));
    browser->onServiceRemoved.connect(
            boost::bind(&LinkLocalPresenceManager::handleServiceRemoved, this, _1));
}

boost::optional<LinkLocalService> LinkLocalPresenceManager::getServiceForJID(const JID& j) const {
    for (const auto& service : browser->getServices()) {
        if (service.getJID() == j) {
            return service;
        }
    }
    return boost::optional<LinkLocalService>();
}

void LinkLocalPresenceManager::handleServiceAdded(const LinkLocalService& service) {
    std::shared_ptr<RosterPayload> roster(new RosterPayload());
    roster->addItem(getRosterItem(service));
    onRosterChanged(roster);
    onPresenceChanged(getPresence(service));
}

void LinkLocalPresenceManager::handleServiceChanged(const LinkLocalService& service) {
    onPresenceChanged(getPresence(service));
}

void LinkLocalPresenceManager::handleServiceRemoved(const LinkLocalService& service) {
    std::shared_ptr<RosterPayload> roster(new RosterPayload());
    roster->addItem(RosterItemPayload(service.getJID(), "", RosterItemPayload::Remove));
    onRosterChanged(roster);
}

std::shared_ptr<RosterPayload> LinkLocalPresenceManager::getRoster() const {
    std::shared_ptr<RosterPayload> roster(new RosterPayload());
    for (const auto& service : browser->getServices()) {
        roster->addItem(getRosterItem(service));
    }
    return roster;
}

std::vector<std::shared_ptr<Presence> > LinkLocalPresenceManager::getAllPresence() const {
    std::vector<std::shared_ptr<Presence> > result;
    for (const auto& service : browser->getServices()) {
        result.push_back(getPresence(service));
    }
    return result;
}

RosterItemPayload LinkLocalPresenceManager::getRosterItem(const LinkLocalService& service) const {
    return RosterItemPayload(service.getJID(), getRosterName(service), RosterItemPayload::Both);
}

std::string LinkLocalPresenceManager::getRosterName(const LinkLocalService& service) const {
    LinkLocalServiceInfo info = service.getInfo();
    if (!info.getNick().empty()) {
        return info.getNick();
    }
    else if (!info.getFirstName().empty()) {
        std::string result = info.getFirstName();
        if (!info.getLastName().empty()) {
            result += " " + info.getLastName();
        }
        return result;
    }
    else if (!info.getLastName().empty()) {
        return info.getLastName();
    }
    return "";
}

std::shared_ptr<Presence> LinkLocalPresenceManager::getPresence(const LinkLocalService& service) const {
    std::shared_ptr<Presence> presence(new Presence());
    presence->setFrom(service.getJID());
    switch (service.getInfo().getStatus()) {
        case LinkLocalServiceInfo::Available:
            presence->setShow(StatusShow::Online);
            break;
        case LinkLocalServiceInfo::Away:
            presence->setShow(StatusShow::Away);
            break;
        case LinkLocalServiceInfo::DND:
            presence->setShow(StatusShow::DND);
            break;
    }
    presence->setStatus(service.getInfo().getMessage());
    return presence;
}

}
