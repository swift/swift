/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>


#include <Swiften/JID/JID.h>

namespace Swift {
	class SWIFTEN_API PubSubOwnerSubscription : public Payload {
		public:
			enum SubscriptionType {
				None,
				Pending,
				Subscribed,
				Unconfigured
			};

			PubSubOwnerSubscription();
			
			virtual ~PubSubOwnerSubscription();

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


		private:
			JID jid;
			SubscriptionType subscription;
	};
}
