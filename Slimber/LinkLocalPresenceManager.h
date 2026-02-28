/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class LinkLocalService;
    class LinkLocalServiceBrowser;
    class RosterPayload;
    class Presence;

    class LinkLocalPresenceManager : public boost::signals2::trackable {
        public:
            LinkLocalPresenceManager(LinkLocalServiceBrowser*);

            std::shared_ptr<RosterPayload> getRoster() const;
            std::vector<std::shared_ptr<Presence> > getAllPresence() const;

            boost::optional<LinkLocalService> getServiceForJID(const JID&) const;

            boost::signals2::signal<void (std::shared_ptr<RosterPayload>)> onRosterChanged;
            boost::signals2::signal<void (std::shared_ptr<Presence>)> onPresenceChanged;

        private:
            void handleServiceAdded(const LinkLocalService&);
            void handleServiceChanged(const LinkLocalService&);
            void handleServiceRemoved(const LinkLocalService&);

            RosterItemPayload getRosterItem(const LinkLocalService& service) const;
            std::string getRosterName(const LinkLocalService& service) const;
            std::shared_ptr<Presence> getPresence(const LinkLocalService& service) const;

        private:
            LinkLocalServiceBrowser* browser;
    };
}
