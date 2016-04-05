/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include <boost/signals2.hpp>

namespace Swift {
    class JID;
    class VCardManager;

    class ContactEditWindow {
        public:
            virtual ~ContactEditWindow() {}

            virtual void setEnabled(bool b) = 0;

            virtual void setNameSuggestions(const std::vector<std::string>& suggestions) = 0;
            virtual void setContact(const JID& jid, const std::string& name, const std::vector<std::string>& groups, const std::set<std::string>& allGroups) = 0;

            virtual void show() = 0;
            virtual void hide() = 0;

            boost::signals2::signal<void ()> onRemoveContactRequest;
            boost::signals2::signal<void (const std::string& /* name */, const std::set<std::string>& /* groups */)> onChangeContactRequest;
    };
}
