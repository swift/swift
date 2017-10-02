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
#include <Swiften/Elements/PubSubOwnerPayload.h>
#include <Swiften/Elements/PubSubOwnerRedirect.h>

namespace Swift {
    class SWIFTEN_API PubSubOwnerDelete : public PubSubOwnerPayload {
        public:

            PubSubOwnerDelete();
            PubSubOwnerDelete(const std::string& node) : node(node) {}
            virtual ~PubSubOwnerDelete();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            std::shared_ptr<PubSubOwnerRedirect> getRedirect() const {
                return redirect;
            }

            void setRedirect(std::shared_ptr<PubSubOwnerRedirect> value) {
                this->redirect = value ;
            }


        private:
            std::string node;
            std::shared_ptr<PubSubOwnerRedirect> redirect;
    };
}
