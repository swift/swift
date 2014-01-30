/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/PubSubPayload.h>
#include <Swiften/Elements/PubSubSubscribeOptions.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class SWIFTEN_API PubSubSubscription : public PubSubPayload {
		public:
			enum SubscriptionType {
				None,
				Pending,
				Subscribed,
				Unconfigured
			};

			PubSubSubscription();
			
			virtual ~PubSubSubscription();

			const boost::optional< std::string >& getNode() const {
				return node;
			}

			void setNode(const boost::optional< std::string >& value) {
				this->node = value ;
			}

			const boost::optional< std::string >& getSubscriptionID() const {
				return subscriptionID;
			}

			void setSubscriptionID(const boost::optional< std::string >& value) {
				this->subscriptionID = value ;
			}

			const JID& getJID() const {
				return jid;
			}

			void setJID(const JID& value) {
				this->jid = value ;
			}

			boost::shared_ptr<PubSubSubscribeOptions> getOptions() const {
				return options;
			}

			void setOptions(boost::shared_ptr<PubSubSubscribeOptions> value) {
				this->options = value ;
			}

			SubscriptionType getSubscription() const {
				return subscription;
			}

			void setSubscription(SubscriptionType value) {
				this->subscription = value ;
			}


		private:
			boost::optional< std::string > node;
			boost::optional< std::string > subscriptionID;
			JID jid;
			boost::shared_ptr<PubSubSubscribeOptions> options;
			SubscriptionType subscription;
	};
}
