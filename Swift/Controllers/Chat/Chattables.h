/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Elements/StatusShow.h>
#include <Swiften/JID/JID.h>

namespace Swift {
class Chattables {
    public:
        struct State {
            enum class Type {Room, Person};
            JID jid;
            /// Empty for no name
            std::string name;
            size_t unreadCount = 0;
            Type type;
            StatusShow::Type status = StatusShow::None;
            //avatar
            //status
        };
        const std::vector<JID>& get() const;
        const State& getState(const JID& jid) const;

        void addJID(const JID& jid, State::Type type);
        void setState(const JID& jid, State state);

        boost::signals2::signal<void (int)> onBeginAdd;
        boost::signals2::signal<void ()> onAdded;
        boost::signals2::signal<void (const JID&, int)> onChanged;
        /// The UI has activated a chattable item (e.g. clicked in the roster)
        boost::signals2::signal<void (const JID&)> onActivated;
    private:
        std::vector<JID> list_;
        std::map<JID, State> states_;
        State unknown_;
};
}
