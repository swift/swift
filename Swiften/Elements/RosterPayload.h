/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/RosterItemPayload.h>

namespace Swift {
    class SWIFTEN_API RosterPayload : public Payload {
        public:
            typedef std::shared_ptr<RosterPayload> ref;
            typedef std::vector<RosterItemPayload> RosterItemPayloads;

        public:
            RosterPayload(bool hasAnnotate = false) : hasAnnotate_(hasAnnotate) {}

            boost::optional<RosterItemPayload> getItem(const JID& jid) const;

            void addItem(const RosterItemPayload& item) {
                items_.push_back(item);
            }

            const RosterItemPayloads& getItems() const {
                return items_;
            }

            const boost::optional<std::string>& getVersion() const {
                return version_;
            }

            void setVersion(const std::string& version) {
                version_ = version;
            }

            bool hasMIXAnnotationSupport() const {
                return hasAnnotate_;
            }

            void setSupportsMIXAnnotations(bool hasAnnotate) {
                hasAnnotate_ = hasAnnotate;
            }

        private:
            bool hasAnnotate_;
            RosterItemPayloads items_;
            boost::optional<std::string> version_;
    };
}
