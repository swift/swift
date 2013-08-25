/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include <Swiften/Elements/PubSubSubscription.h>
#include <Swiften/Elements/PubSubPayload.h>

namespace Swift {
	class SWIFTEN_API PubSubSubscriptions : public PubSubPayload {
		public:
			
			PubSubSubscriptions();
			PubSubSubscriptions(const std::string& node) : node(node) {}
			virtual ~PubSubSubscriptions();

			const boost::optional< std::string >& getNode() const {
				return node;
			}

			void setNode(const boost::optional< std::string >& value) {
				this->node = value ;
			}

			const std::vector< boost::shared_ptr<PubSubSubscription> >& getSubscriptions() const {
				return subscriptions;
			}

			void setSubscriptions(const std::vector< boost::shared_ptr<PubSubSubscription> >& value) {
				this->subscriptions = value ;
			}

			void addSubscription(boost::shared_ptr<PubSubSubscription> value) {
				this->subscriptions.push_back(value);
			}


		private:
			boost::optional< std::string > node;
			std::vector< boost::shared_ptr<PubSubSubscription> > subscriptions;
	};
}
