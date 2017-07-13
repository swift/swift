/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubEventPayload.h>
#include <Swiften/Elements/PubSubEventRedirect.h>

namespace Swift {
    class SWIFTEN_API PubSubEventDelete : public PubSubEventPayload {
        public:

            PubSubEventDelete();

            virtual ~PubSubEventDelete();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            std::shared_ptr<PubSubEventRedirect> getRedirects() const {
                return redirects;
            }

            void setRedirects(std::shared_ptr<PubSubEventRedirect> value) {
                this->redirects = value ;
            }


        private:
            std::string node;
            std::shared_ptr<PubSubEventRedirect> redirects;
    };
}
