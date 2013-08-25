/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include <Swiften/Elements/PubSubOwnerPayload.h>
#include <Swiften/Elements/PubSubOwnerAffiliation.h>

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

			const std::vector< boost::shared_ptr<PubSubOwnerAffiliation> >& getAffiliations() const {
				return affiliations;
			}

			void setAffiliations(const std::vector< boost::shared_ptr<PubSubOwnerAffiliation> >& value) {
				this->affiliations = value ;
			}

			void addAffiliation(boost::shared_ptr<PubSubOwnerAffiliation> value) {
				this->affiliations.push_back(value);
			}


		private:
			std::string node;
			std::vector< boost::shared_ptr<PubSubOwnerAffiliation> > affiliations;
	};
}
