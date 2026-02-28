/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubAffiliations.h>
#include <Swiften/Elements/PubSubCreate.h>
#include <Swiften/Elements/PubSubDefault.h>
#include <Swiften/Elements/PubSubEventPayload.h>
#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/Elements/PubSubOwnerAffiliations.h>
#include <Swiften/Elements/PubSubOwnerConfigure.h>
#include <Swiften/Elements/PubSubOwnerDefault.h>
#include <Swiften/Elements/PubSubOwnerDelete.h>
#include <Swiften/Elements/PubSubOwnerPubSub.h>
#include <Swiften/Elements/PubSubOwnerPurge.h>
#include <Swiften/Elements/PubSubOwnerSubscriptions.h>
#include <Swiften/Elements/PubSubPublish.h>
#include <Swiften/Elements/PubSubRetract.h>
#include <Swiften/Elements/PubSubSubscribe.h>
#include <Swiften/Elements/PubSubSubscription.h>
#include <Swiften/Elements/PubSubSubscriptions.h>
#include <Swiften/Elements/PubSubUnsubscribe.h>
#include <Swiften/PubSub/PubSubUtil.h>
#include <Swiften/Queries/PubSubRequest.h>
#define SWIFTEN_PUBSUBMANAGER_DECLARE_CREATE_REQUEST(payload, container, response) \
    virtual std::shared_ptr< PubSubRequest<payload> >  \
        createRequest(IQ::Type, const JID&, std::shared_ptr<payload>) = 0;

namespace Swift {
    class JID;

    class SWIFTEN_API PubSubManager {
        public:
            virtual ~PubSubManager();

            SWIFTEN_PUBSUB_FOREACH_PUBSUB_PAYLOAD_TYPE(
                    SWIFTEN_PUBSUBMANAGER_DECLARE_CREATE_REQUEST)

            boost::signals2::signal<void (const JID&, const std::shared_ptr<PubSubEventPayload>)> onEvent;
    };
}
