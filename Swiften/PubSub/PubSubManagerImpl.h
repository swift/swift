/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/PubSub/PubSubManager.h>

#define SWIFTEN_PUBSUBMANAGERIMPL_DECLARE_CREATE_REQUEST(payload, container, response) \
	virtual boost::shared_ptr< PubSubRequest<payload> >  \
			createRequest(IQ::Type type, const JID& receiver, boost::shared_ptr<payload> p) SWIFTEN_OVERRIDE { \
		return boost::make_shared< PubSubRequest<payload> >(type, receiver, p, router); \
	}

namespace Swift {
	class JID;
	class StanzaChannel;
	class Message;

	class SWIFTEN_API PubSubManagerImpl : public PubSubManager {
		public:
			PubSubManagerImpl(StanzaChannel* stanzaChannel, IQRouter* router);
			virtual ~PubSubManagerImpl();

			SWIFTEN_PUBSUB_FOREACH_PUBSUB_PAYLOAD_TYPE(
					SWIFTEN_PUBSUBMANAGERIMPL_DECLARE_CREATE_REQUEST)

		private:
			void handleMessageRecevied(boost::shared_ptr<Message>);

		private:
			StanzaChannel* stanzaChannel;
			IQRouter* router;
	};
}
