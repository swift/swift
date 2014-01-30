/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>
#include <string>

#include <Swiften/Elements/PubSubEventPayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class SWIFTEN_API PubSubEventSubscription : public PubSubEventPayload {
		public:
			enum SubscriptionType {
				None,
				Pending,
				Subscribed,
				Unconfigured
			};

			PubSubEventSubscription();
			
			virtual ~PubSubEventSubscription();

			const std::string& getNode() const {
				return node;
			}

			void setNode(const std::string& value) {
				this->node = value ;
			}

			const JID& getJID() const {
				return jid;
			}

			void setJID(const JID& value) {
				this->jid = value ;
			}

			SubscriptionType getSubscription() const {
				return subscription;
			}

			void setSubscription(SubscriptionType value) {
				this->subscription = value ;
			}

			const boost::optional< std::string >& getSubscriptionID() const {
				return subscriptionID;
			}

			void setSubscriptionID(const boost::optional< std::string >& value) {
				this->subscriptionID = value ;
			}

			const boost::posix_time::ptime& getExpiry() const {
				return expiry;
			}

			void setExpiry(const boost::posix_time::ptime& value) {
				this->expiry = value ;
			}


		private:
			std::string node;
			JID jid;
			SubscriptionType subscription;
			boost::optional< std::string > subscriptionID;
			boost::posix_time::ptime expiry;
	};
}
