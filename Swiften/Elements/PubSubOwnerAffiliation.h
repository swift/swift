/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API PubSubOwnerAffiliation : public Payload {
        public:
            enum Type {
                None,
                Member,
                Outcast,
                Owner,
                Publisher,
                PublishOnly
            };

            PubSubOwnerAffiliation();

            virtual ~PubSubOwnerAffiliation();

            const JID& getJID() const {
                return jid;
            }

            void setJID(const JID& value) {
                this->jid = value ;
            }

            Type getType() const {
                return type;
            }

            void setType(Type value) {
                this->type = value ;
            }


        private:
            JID jid;
            Type type;
    };
}
