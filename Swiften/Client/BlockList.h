/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API BlockList {
        public:
            enum State {
                Init,
                Requesting,
                Available,
                Error
            };
            virtual ~BlockList();

            virtual State getState() const = 0;

            virtual const std::vector<JID>& getItems() const = 0;

            bool isBlocked(const JID& jid) const;

        public:
            boost::signals2::signal<void ()> onStateChanged;
            boost::signals2::signal<void (const JID&)> onItemAdded;
            boost::signals2::signal<void (const JID&)> onItemRemoved;
    };
}
