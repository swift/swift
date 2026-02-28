/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Chat/MUCSearchController.h>

namespace Swift {

    class MUCSearchWindow {
        public:
            virtual ~MUCSearchWindow() {}

            virtual void clearList() = 0;
            virtual void addService(const MUCService& service) = 0;
            virtual void addSavedServices(const std::list<JID>& services) = 0;
            virtual void setSearchInProgress(bool searching) = 0;

            virtual void show() = 0;

            boost::signals2::signal<void (const JID&)> onSearchService;
            boost::signals2::signal<void (const boost::optional<JID>&)> onFinished;
    };
}
