/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/PubSub/PubSubManager.h>
#define SWIFTEN_PUBSUBMANAGERIMPL_DECLARE_CREATE_REQUEST(payload, container, response) \
    virtual std::shared_ptr< PubSubRequest<payload> >  \
            createRequest(IQ::Type type, const JID& receiver, std::shared_ptr<payload> p) override { \
        return std::make_shared< PubSubRequest<payload> >(type, receiver, p, router); \
    }

namespace Swift {
    class StanzaChannel;
    class Message;

    class SWIFTEN_API PubSubManagerImpl : public PubSubManager {
        public:
            PubSubManagerImpl(StanzaChannel* stanzaChannel, IQRouter* router);
            virtual ~PubSubManagerImpl() override;

            SWIFTEN_PUBSUB_FOREACH_PUBSUB_PAYLOAD_TYPE(
                    SWIFTEN_PUBSUBMANAGERIMPL_DECLARE_CREATE_REQUEST)

        private:
            void handleMessageRecevied(std::shared_ptr<Message>);

        private:
            StanzaChannel* stanzaChannel;
            IQRouter* router;
    };
}
