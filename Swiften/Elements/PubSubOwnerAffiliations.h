/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubOwnerAffiliation.h>
#include <Swiften/Elements/PubSubOwnerPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubOwnerAffiliations : public PubSubOwnerPayload {
        public:

            PubSubOwnerAffiliations();

            virtual ~PubSubOwnerAffiliations();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            const std::vector< std::shared_ptr<PubSubOwnerAffiliation> >& getAffiliations() const {
                return affiliations;
            }

            void setAffiliations(const std::vector< std::shared_ptr<PubSubOwnerAffiliation> >& value) {
                this->affiliations = value ;
            }

            void addAffiliation(std::shared_ptr<PubSubOwnerAffiliation> value) {
                this->affiliations.push_back(value);
            }


        private:
            std::string node;
            std::vector< std::shared_ptr<PubSubOwnerAffiliation> > affiliations;
    };
}
