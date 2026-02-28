/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once
#define SWIFTEN_PUBSUB_FOREACH_PUBSUB_PAYLOAD_TYPE(action) \
    action(PubSubCreate, PubSub, PubSubCreate) \
    action(PubSubAffiliations, PubSub, PubSubAffiliations) \
    action(PubSubDefault, PubSub, PubSubDefault) \
    action(PubSubItems, PubSub, PubSubItems) \
    action(PubSubOptions, PubSub, PubSubOptions) \
    action(PubSubPublish, PubSub, PubSubPublish) \
    action(PubSubRetract, PubSub, PubSubRetract) \
    action(PubSubSubscription, PubSub, PubSubSubscription) \
    action(PubSubSubscriptions, PubSub, PubSubSubscriptions) \
    action(PubSubSubscribe, PubSub, PubSubSubscription) \
    action(PubSubUnsubscribe, PubSub, PubSubUnsubscribe) \
    action(PubSubOwnerAffiliations, PubSubOwnerPubSub, PubSubOwnerAffiliations) \
    action(PubSubOwnerConfigure, PubSubOwnerPubSub, PubSubOwnerConfigure) \
    action(PubSubOwnerDefault, PubSubOwnerPubSub, PubSubOwnerDefault) \
    action(PubSubOwnerDelete, PubSubOwnerPubSub, PubSubOwnerDelete) \
    action(PubSubOwnerPurge, PubSubOwnerPubSub, PubSubOwnerPurge) \
    action(PubSubOwnerSubscriptions, PubSubOwnerPubSub, PubSubOwnerSubscriptions)

