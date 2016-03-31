/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/IsodeIQDelegationConvertor.h>
#include <Sluift/ElementConvertors/PubSubAffiliationConvertor.h>
#include <Sluift/ElementConvertors/PubSubAffiliationsConvertor.h>
#include <Sluift/ElementConvertors/PubSubConfigureConvertor.h>
#include <Sluift/ElementConvertors/PubSubCreateConvertor.h>
#include <Sluift/ElementConvertors/PubSubDefaultConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventAssociateConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventCollectionConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventConfigurationConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventDeleteConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventDisassociateConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventItemConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventItemsConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventPurgeConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventRedirectConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventRetractConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventSubscriptionConvertor.h>
#include <Sluift/ElementConvertors/PubSubItemConvertor.h>
#include <Sluift/ElementConvertors/PubSubItemsConvertor.h>
#include <Sluift/ElementConvertors/PubSubOptionsConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerAffiliationConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerAffiliationsConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerConfigureConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerDefaultConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerDeleteConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerPurgeConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerRedirectConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerSubscriptionConvertor.h>
#include <Sluift/ElementConvertors/PubSubOwnerSubscriptionsConvertor.h>
#include <Sluift/ElementConvertors/PubSubPublishConvertor.h>
#include <Sluift/ElementConvertors/PubSubRetractConvertor.h>
#include <Sluift/ElementConvertors/PubSubSubscribeConvertor.h>
#include <Sluift/ElementConvertors/PubSubSubscribeOptionsConvertor.h>
#include <Sluift/ElementConvertors/PubSubSubscriptionConvertor.h>
#include <Sluift/ElementConvertors/PubSubSubscriptionsConvertor.h>
#include <Sluift/ElementConvertors/PubSubUnsubscribeConvertor.h>
#include <Sluift/ElementConvertors/SecurityLabelConvertor.h>
#include <Sluift/ElementConvertors/UserLocationConvertor.h>
#include <Sluift/ElementConvertors/UserTuneConvertor.h>

void LuaElementConvertors::registerConvertors() {
    convertors.push_back(boost::make_shared<PubSubRetractConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubAffiliationsConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubPublishConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubItemsConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubOwnerRedirectConvertor>());
    convertors.push_back(boost::make_shared<PubSubEventRedirectConvertor>());
    convertors.push_back(boost::make_shared<UserTuneConvertor>());
    convertors.push_back(boost::make_shared<PubSubConfigureConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubEventDisassociateConvertor>());
    convertors.push_back(boost::make_shared<PubSubOwnerAffiliationsConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubOwnerConfigureConvertor>(this));
    convertors.push_back(boost::make_shared<UserLocationConvertor>());
    convertors.push_back(boost::make_shared<PubSubSubscribeOptionsConvertor>());
    convertors.push_back(boost::make_shared<PubSubOwnerSubscriptionsConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubDefaultConvertor>());
    convertors.push_back(boost::make_shared<PubSubEventCollectionConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubEventSubscriptionConvertor>());
    convertors.push_back(boost::make_shared<PubSubEventRetractConvertor>());
    convertors.push_back(boost::make_shared<PubSubItemConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubUnsubscribeConvertor>());
    convertors.push_back(boost::make_shared<PubSubEventDeleteConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubCreateConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubOwnerPurgeConvertor>());
    convertors.push_back(boost::make_shared<PubSubEventItemsConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubOptionsConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubEventItemConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubOwnerSubscriptionConvertor>());
    convertors.push_back(boost::make_shared<IsodeIQDelegationConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubOwnerAffiliationConvertor>());
    convertors.push_back(boost::make_shared<PubSubEventPurgeConvertor>());
    convertors.push_back(boost::make_shared<PubSubAffiliationConvertor>());
    convertors.push_back(boost::make_shared<PubSubSubscribeConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubOwnerDeleteConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubOwnerDefaultConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubSubscriptionsConvertor>(this));
    convertors.push_back(boost::make_shared<PubSubEventAssociateConvertor>());
    convertors.push_back(boost::make_shared<PubSubSubscriptionConvertor>(this));
    convertors.push_back(boost::make_shared<SecurityLabelConvertor>());
    convertors.push_back(boost::make_shared<PubSubEventConfigurationConvertor>(this));
}
