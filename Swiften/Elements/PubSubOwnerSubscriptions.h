/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubOwnerPayload.h>
#include <Swiften/Elements/PubSubOwnerSubscription.h>

namespace Swift {
	class SWIFTEN_API PubSubOwnerSubscriptions : public PubSubOwnerPayload {
		public:
			
			PubSubOwnerSubscriptions();
			
			virtual ~PubSubOwnerSubscriptions();

			const std::string& getNode() const {
				return node;
			}

			void setNode(const std::string& value) {
				this->node = value ;
			}

			const std::vector< boost::shared_ptr<PubSubOwnerSubscription> >& getSubscriptions() const {
				return subscriptions;
			}

			void setSubscriptions(const std::vector< boost::shared_ptr<PubSubOwnerSubscription> >& value) {
				this->subscriptions = value ;
			}

			void addSubscription(boost::shared_ptr<PubSubOwnerSubscription> value) {
				this->subscriptions.push_back(value);
			}


		private:
			std::string node;
			std::vector< boost::shared_ptr<PubSubOwnerSubscription> > subscriptions;
	};
}
