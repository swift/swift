/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubErrorSerializer.h>

#include <cassert>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubErrorSerializer::PubSubErrorSerializer() {
}

PubSubErrorSerializer::~PubSubErrorSerializer() {
}

std::string PubSubErrorSerializer::serializePayload(std::shared_ptr<PubSubError> payload) const {
    if (payload->getType() == PubSubError::UnknownType) {
        return "";
    }
    XMLElement element(serializeType(payload->getType()), "http://jabber.org/protocol/pubsub#errors");
    if (payload->getType() == PubSubError::Unsupported) {
        if (payload->getUnsupportedFeatureType() != PubSubError::UnknownUnsupportedFeatureType) {
            element.setAttribute("feature", serializeUnsupportedFeatureType(payload->getUnsupportedFeatureType()));
        }
    }
    return element.serialize();
}

std::string PubSubErrorSerializer::serializeType(PubSubError::Type value) {
    switch (value) {
        case PubSubError::UnknownType: assert(false); return "";
        case PubSubError::ClosedNode: return "closed-node";
        case PubSubError::ConfigurationRequired: return "configuration-required";
        case PubSubError::InvalidJID: return "invalid-jid";
        case PubSubError::InvalidOptions: return "invalid-options";
        case PubSubError::InvalidPayload: return "invalid-payload";
        case PubSubError::InvalidSubscriptionID: return "invalid-subid";
        case PubSubError::ItemForbidden: return "item-forbidden";
        case PubSubError::ItemRequired: return "item-required";
        case PubSubError::JIDRequired: return "jid-required";
        case PubSubError::MaximumItemsExceeded: return "max-items-exceeded";
        case PubSubError::MaximumNodesExceeded: return "max-nodes-exceeded";
        case PubSubError::NodeIDRequired: return "nodeid-required";
        case PubSubError::NotInRosterGroup: return "not-in-roster-group";
        case PubSubError::NotSubscribed: return "not-subscribed";
        case PubSubError::PayloadTooBig: return "payload-too-big";
        case PubSubError::PayloadRequired: return "payload-required";
        case PubSubError::PendingSubscription: return "pending-subscription";
        case PubSubError::PresenceSubscriptionRequired: return "presence-subscription-required";
        case PubSubError::SubscriptionIDRequired: return "subid-required";
        case PubSubError::TooManySubscriptions: return "too-many-subscriptions";
        case PubSubError::Unsupported: return "unsupported";
        case PubSubError::UnsupportedAccessModel: return "unsupported-access-model";
    }
    assert(false);
    return "";
}

std::string PubSubErrorSerializer::serializeUnsupportedFeatureType(PubSubError::UnsupportedFeatureType value) {
    switch (value) {
        case PubSubError::UnknownUnsupportedFeatureType: assert(false); return "";
        case PubSubError::AccessAuthorize: return "access-authorize";
        case PubSubError::AccessOpen: return "access-open";
        case PubSubError::AccessPresence: return "access-presence";
        case PubSubError::AccessRoster: return "access-roster";
        case PubSubError::AccessWhitelist: return "access-whitelist";
        case PubSubError::AutoCreate: return "auto-create";
        case PubSubError::AutoSubscribe: return "auto-subscribe";
        case PubSubError::Collections: return "collections";
        case PubSubError::ConfigNode: return "config-node";
        case PubSubError::CreateAndConfigure: return "create-and-configure";
        case PubSubError::CreateNodes: return "create-nodes";
        case PubSubError::DeleteItems: return "delete-items";
        case PubSubError::DeleteNodes: return "delete-nodes";
        case PubSubError::FilteredNotifications: return "filtered-notifications";
        case PubSubError::GetPending: return "get-pending";
        case PubSubError::InstantNodes: return "instant-nodes";
        case PubSubError::ItemIDs: return "item-ids";
        case PubSubError::LastPublished: return "last-published";
        case PubSubError::LeasedSubscription: return "leased-subscription";
        case PubSubError::ManageSubscriptions: return "manage-subscriptions";
        case PubSubError::MemberAffiliation: return "member-affiliation";
        case PubSubError::MetaData: return "meta-data";
        case PubSubError::ModifyAffiliations: return "modify-affiliations";
        case PubSubError::MultiCollection: return "multi-collection";
        case PubSubError::MultiSubscribe: return "multi-subscribe";
        case PubSubError::OutcastAffiliation: return "outcast-affiliation";
        case PubSubError::PersistentItems: return "persistent-items";
        case PubSubError::PresenceNotifications: return "presence-notifications";
        case PubSubError::PresenceSubscribe: return "presence-subscribe";
        case PubSubError::Publish: return "publish";
        case PubSubError::PublishOptions: return "publish-options";
        case PubSubError::PublishOnlyAffiliation: return "publish-only-affiliation";
        case PubSubError::PublisherAffiliation: return "publisher-affiliation";
        case PubSubError::PurgeNodes: return "purge-nodes";
        case PubSubError::RetractItems: return "retract-items";
        case PubSubError::RetrieveAffiliations: return "retrieve-affiliations";
        case PubSubError::RetrieveDefault: return "retrieve-default";
        case PubSubError::RetrieveItems: return "retrieve-items";
        case PubSubError::RetrieveSubscriptions: return "retrieve-subscriptions";
        case PubSubError::Subscribe: return "subscribe";
        case PubSubError::SubscriptionOptions: return "subscription-options";
        case PubSubError::SubscriptionNotifications: return "subscription-notifications";
    }
    assert(false);
    return "";
}
