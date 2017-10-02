/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubOptions.h>
#include <Swiften/Elements/PubSubPayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API PubSubSubscribe : public PubSubPayload {
        public:

            PubSubSubscribe();

            virtual ~PubSubSubscribe();

            const boost::optional< std::string >& getNode() const {
                return node;
            }

            void setNode(const boost::optional< std::string >& value) {
                this->node = value ;
            }

            const JID& getJID() const {
                return jid;
            }

            void setJID(const JID& value) {
                this->jid = value ;
            }

            std::shared_ptr<PubSubOptions> getOptions() const {
                return options;
            }

            void setOptions(std::shared_ptr<PubSubOptions> value) {
                this->options = value ;
            }


        private:
            boost::optional< std::string > node;
            JID jid;
            std::shared_ptr<PubSubOptions> options;
    };
}
