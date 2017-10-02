/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API PubSubError : public Payload {
        public:
            enum Type {
                UnknownType = 0,
                ClosedNode,
                ConfigurationRequired,
                InvalidJID,
                InvalidOptions,
                InvalidPayload,
                InvalidSubscriptionID,
                ItemForbidden,
                ItemRequired,
                JIDRequired,
                MaximumItemsExceeded,
                MaximumNodesExceeded,
                NodeIDRequired,
                NotInRosterGroup,
                NotSubscribed,
                PayloadTooBig,
                PayloadRequired,
                PendingSubscription,
                PresenceSubscriptionRequired,
                SubscriptionIDRequired,
                TooManySubscriptions,
                Unsupported,
                UnsupportedAccessModel
            };

            enum UnsupportedFeatureType {
                UnknownUnsupportedFeatureType = 0,
                AccessAuthorize,
                AccessOpen,
                AccessPresence,
                AccessRoster,
                AccessWhitelist,
                AutoCreate,
                AutoSubscribe,
                Collections,
                ConfigNode,
                CreateAndConfigure,
                CreateNodes,
                DeleteItems,
                DeleteNodes,
                FilteredNotifications,
                GetPending,
                InstantNodes,
                ItemIDs,
                LastPublished,
                LeasedSubscription,
                ManageSubscriptions,
                MemberAffiliation,
                MetaData,
                ModifyAffiliations,
                MultiCollection,
                MultiSubscribe,
                OutcastAffiliation,
                PersistentItems,
                PresenceNotifications,
                PresenceSubscribe,
                Publish,
                PublishOptions,
                PublishOnlyAffiliation,
                PublisherAffiliation,
                PurgeNodes,
                RetractItems,
                RetrieveAffiliations,
                RetrieveDefault,
                RetrieveItems,
                RetrieveSubscriptions,
                Subscribe,
                SubscriptionOptions,
                SubscriptionNotifications
            };

            PubSubError(Type type = UnknownType) : type(type), unsupportedType(UnknownUnsupportedFeatureType) {
            }

            virtual ~PubSubError();

            Type getType() const {
                return type;
            }

            void setType(Type type) {
                this->type = type;
            }

            UnsupportedFeatureType getUnsupportedFeatureType() const {
                return unsupportedType;
            }

            void setUnsupportedFeatureType(UnsupportedFeatureType unsupportedType) {
                this->unsupportedType = unsupportedType;
            }

        private:
            Type type;
            UnsupportedFeatureType unsupportedType;
    };
}
