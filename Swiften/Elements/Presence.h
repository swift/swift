/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Elements/StatusShow.h>

namespace Swift {
    class SWIFTEN_API Presence : public Stanza {
        public:
            typedef std::shared_ptr<Presence> ref;

            enum Type { Available, Error, Probe, Subscribe, Subscribed, Unavailable, Unsubscribe, Unsubscribed };

            Presence();
            Presence(const std::string& status);
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Presence)
            virtual ~Presence();

            static ref create() {
                return std::make_shared<Presence>();
            }

            static ref create(const std::string& status) {
                return std::make_shared<Presence>(status);
            }

            static ref create(Presence::ref presence) {
                return std::make_shared<Presence>(*presence);
            }

            Type getType() const { return type_; }
            void setType(Type type) { type_ = type; }

            StatusShow::Type getShow() const {
                std::shared_ptr<StatusShow> show(getPayload<StatusShow>());
                if (show) {
                    return show->getType();
                }
                return type_ == Available ? StatusShow::Online : StatusShow::None;
            }

            void setShow(const StatusShow::Type &show) {
                updatePayload(std::make_shared<StatusShow>(show));
            }

            std::string getStatus() const;
            void setStatus(const std::string& status);

            int getPriority() const;
            void setPriority(int priority);

            std::shared_ptr<Presence> clone() const {
                return std::make_shared<Presence>(*this);
            }

            bool isAvailable() const {
                return type_ == Available;
            }

        private:
            Presence::Type type_;
    };
}
