/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/ContainerPayload.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubAffiliations.h>
#include <Swiften/Elements/PubSubCreate.h>
#include <Swiften/Elements/PubSubDefault.h>
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
#include <Swiften/Queries/Request.h>

namespace Swift {
    namespace Detail {
        template<typename T>
        struct PubSubPayloadTraits;

#define SWIFTEN_PUBSUB_DECLARE_PAYLOAD_TRAITS(PAYLOAD, CONTAINER, RESPONSE) \
        template<> struct PubSubPayloadTraits< PAYLOAD > { \
            typedef CONTAINER ContainerType; \
            typedef RESPONSE ResponseType; \
        };

        SWIFTEN_PUBSUB_FOREACH_PUBSUB_PAYLOAD_TYPE(
                SWIFTEN_PUBSUB_DECLARE_PAYLOAD_TRAITS)
    }

    template<typename T>
    class SWIFTEN_API PubSubRequest : public Request {
            typedef typename Detail::PubSubPayloadTraits<T>::ContainerType ContainerType;
            typedef typename Detail::PubSubPayloadTraits<T>::ResponseType ResponseType;

        public:
            PubSubRequest(
                    IQ::Type type,
                    const JID& receiver,
                    boost::shared_ptr<T> payload,
                    IQRouter* router) :
                        Request(type, receiver, router) {
                boost::shared_ptr<ContainerType> wrapper = boost::make_shared<ContainerType>();
                wrapper->setPayload(payload);
                setPayload(wrapper);
            }

            PubSubRequest(
                    IQ::Type type,
                    const JID& sender,
                    const JID& receiver,
                    boost::shared_ptr<T> payload,
                    IQRouter* router) :
                        Request(type, sender, receiver, router) {
                boost::shared_ptr<ContainerType> wrapper = boost::make_shared<ContainerType>();
                wrapper->setPayload(payload);
                setPayload(wrapper);
            }

            virtual void handleResponse(
                    boost::shared_ptr<Payload> payload, ErrorPayload::ref error) {
                boost::shared_ptr<ResponseType> result;
                if (boost::shared_ptr<ContainerType> container = boost::dynamic_pointer_cast<ContainerType>(payload)) {
                    result = boost::dynamic_pointer_cast<ResponseType>(container->getPayload());
                }
                onResponse(result, error);
            }

        public:
            boost::signal<void (boost::shared_ptr<ResponseType>, ErrorPayload::ref)> onResponse;
    };
}
