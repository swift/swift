/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubAffiliation.h>
#include <Swiften/Elements/PubSubPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubAffiliations : public PubSubPayload {
        public:

            PubSubAffiliations();

            virtual ~PubSubAffiliations();

            const boost::optional< std::string >& getNode() const {
                return node;
            }

            void setNode(const boost::optional< std::string >& value) {
                this->node = value ;
            }

            const std::vector< std::shared_ptr<PubSubAffiliation> >& getAffiliations() const {
                return affiliations;
            }

            void setAffiliations(const std::vector< std::shared_ptr<PubSubAffiliation> >& value) {
                this->affiliations = value ;
            }

            void addAffiliation(std::shared_ptr<PubSubAffiliation> value) {
                this->affiliations.push_back(value);
            }


        private:
            boost::optional< std::string > node;
            std::vector< std::shared_ptr<PubSubAffiliation> > affiliations;
    };
}
