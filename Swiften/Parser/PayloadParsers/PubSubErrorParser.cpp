/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PubSubErrorParser.h>

using namespace Swift;

PubSubErrorParser::PubSubErrorParser() : level(0) {
    typeParser
        (PubSubError::ClosedNode, "closed-node")
        (PubSubError::ConfigurationRequired, "configuration-required")
        (PubSubError::InvalidJID, "invalid-jid")
        (PubSubError::InvalidOptions, "invalid-options")
        (PubSubError::InvalidPayload, "invalid-payload")
        (PubSubError::InvalidSubscriptionID, "invalid-subid")
        (PubSubError::ItemForbidden, "item-forbidden")
        (PubSubError::ItemRequired, "item-required")
        (PubSubError::JIDRequired, "jid-required")
        (PubSubError::MaximumItemsExceeded, "max-items-exceeded")
        (PubSubError::MaximumNodesExceeded, "max-nodes-exceeded")
        (PubSubError::NodeIDRequired, "nodeid-required")
        (PubSubError::NotInRosterGroup, "not-in-roster-group")
        (PubSubError::NotSubscribed, "not-subscribed")
        (PubSubError::PayloadTooBig, "payload-too-big")
        (PubSubError::PayloadRequired, "payload-required")
        (PubSubError::PendingSubscription, "pending-subscription")
        (PubSubError::PresenceSubscriptionRequired, "presence-subscription-required")
        (PubSubError::SubscriptionIDRequired, "subid-required")
        (PubSubError::TooManySubscriptions, "too-many-subscriptions")
        (PubSubError::Unsupported, "unsupported")
        (PubSubError::UnsupportedAccessModel, "unsupported-access-model");
    unsupportedTypeParser
        (PubSubError::AccessAuthorize, "access-authorize")
        (PubSubError::AccessOpen, "access-open")
        (PubSubError::AccessPresence, "access-presence")
        (PubSubError::AccessRoster, "access-roster")
        (PubSubError::AccessWhitelist, "access-whitelist")
        (PubSubError::AutoCreate, "auto-create")
        (PubSubError::AutoSubscribe, "auto-subscribe")
        (PubSubError::Collections, "collections")
        (PubSubError::ConfigNode, "config-node")
        (PubSubError::CreateAndConfigure, "create-and-configure")
        (PubSubError::CreateNodes, "create-nodes")
        (PubSubError::DeleteItems, "delete-items")
        (PubSubError::DeleteNodes, "delete-nodes")
        (PubSubError::FilteredNotifications, "filtered-notifications")
        (PubSubError::GetPending, "get-pending")
        (PubSubError::InstantNodes, "instant-nodes")
        (PubSubError::ItemIDs, "item-ids")
        (PubSubError::LastPublished, "last-published")
        (PubSubError::LeasedSubscription, "leased-subscription")
        (PubSubError::ManageSubscriptions, "manage-subscriptions")
        (PubSubError::MemberAffiliation, "member-affiliation")
        (PubSubError::MetaData, "meta-data")
        (PubSubError::ModifyAffiliations, "modify-affiliations")
        (PubSubError::MultiCollection, "multi-collection")
        (PubSubError::MultiSubscribe, "multi-subscribe")
        (PubSubError::OutcastAffiliation, "outcast-affiliation")
        (PubSubError::PersistentItems, "persistent-items")
        (PubSubError::PresenceNotifications, "presence-notifications")
        (PubSubError::PresenceSubscribe, "presence-subscribe")
        (PubSubError::Publish, "publish")
        (PubSubError::PublishOptions, "publish-options")
        (PubSubError::PublishOnlyAffiliation, "publish-only-affiliation")
        (PubSubError::PublisherAffiliation, "publisher-affiliation")
        (PubSubError::PurgeNodes, "purge-nodes")
        (PubSubError::RetractItems, "retract-items")
        (PubSubError::RetrieveAffiliations, "retrieve-affiliations")
        (PubSubError::RetrieveDefault, "retrieve-default")
        (PubSubError::RetrieveItems, "retrieve-items")
        (PubSubError::RetrieveSubscriptions, "retrieve-subscriptions")
        (PubSubError::Subscribe, "subscribe")
        (PubSubError::SubscriptionOptions, "subscription-options")
        (PubSubError::SubscriptionNotifications, "subscription-notifications");
}

PubSubErrorParser::~PubSubErrorParser() {
}

void PubSubErrorParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
    if (level == 1) {
        if (boost::optional<PubSubError::Type> type = typeParser.parse(element)) {
            getPayloadInternal()->setType(*type);
            if (type == PubSubError::Unsupported) {
                if (boost::optional<std::string> feature = attributes.getAttributeValue("feature")) {
                    if (boost::optional<PubSubError::UnsupportedFeatureType> unsupportedType = unsupportedTypeParser.parse(*feature)) {
                        getPayloadInternal()->setUnsupportedFeatureType(*unsupportedType);
                    }
                }
            }
        }
    }
    ++level;
}

void PubSubErrorParser::handleEndElement(const std::string&, const std::string&) {
    --level;
}

void PubSubErrorParser::handleCharacterData(const std::string&) {
}
