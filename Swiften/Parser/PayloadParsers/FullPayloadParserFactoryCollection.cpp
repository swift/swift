/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Parser/GenericPayloadParser.h"
#include "Swiften/Parser/PayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/ErrorParser.h"
#include "Swiften/Parser/PayloadParsers/BodyParser.h"
#include "Swiften/Parser/PayloadParsers/SubjectParser.h"
#include "Swiften/Parser/PayloadParsers/ChatStateParserFactory.h"
#include "Swiften/Parser/PayloadParsers/PriorityParser.h"
#include "Swiften/Parser/PayloadParsers/ResourceBindParser.h"
#include "Swiften/Parser/PayloadParsers/StartSessionParser.h"
#include "Swiften/Parser/PayloadParsers/StatusParser.h"
#include "Swiften/Parser/PayloadParsers/StatusShowParser.h"
#include "Swiften/Parser/PayloadParsers/RosterParser.h"
#include "Swiften/Parser/PayloadParsers/SoftwareVersionParser.h"
#include "Swiften/Parser/PayloadParsers/StorageParser.h"
#include "Swiften/Parser/PayloadParsers/DiscoInfoParser.h"
#include "Swiften/Parser/PayloadParsers/DiscoItemsParser.h"
#include "Swiften/Parser/PayloadParsers/CapsInfoParser.h"
#include "Swiften/Parser/PayloadParsers/SecurityLabelParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SecurityLabelsCatalogParserFactory.h"
#include "Swiften/Parser/PayloadParsers/FormParserFactory.h"
#include "Swiften/Parser/PayloadParsers/CommandParserFactory.h"
#include "Swiften/Parser/PayloadParsers/InBandRegistrationPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SearchPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StreamInitiationParserFactory.h"
#include "Swiften/Parser/PayloadParsers/BytestreamsParserFactory.h"
#include "Swiften/Parser/PayloadParsers/IBBParser.h"
#include "Swiften/Parser/PayloadParsers/VCardUpdateParserFactory.h"
#include "Swiften/Parser/PayloadParsers/VCardParserFactory.h"
#include "Swiften/Parser/PayloadParsers/RawXMLPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/PrivateStorageParserFactory.h"
#include "Swiften/Parser/PayloadParsers/DelayParserFactory.h"
#include "Swiften/Parser/PayloadParsers/MUCUserPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/NicknameParserFactory.h"

using namespace boost;

namespace Swift {

FullPayloadParserFactoryCollection::FullPayloadParserFactoryCollection() {
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<IBBParser>("", "http://jabber.org/protocol/ibb")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StatusShowParser>("show")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StatusParser>("status")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<BodyParser>("body")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<SubjectParser>("subject")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<PriorityParser>("priority")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<ErrorParser>("error")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<SoftwareVersionParser>("query", "jabber:iq:version")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StorageParser>("storage", "storage:bookmarks")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<RosterParser>("query", "jabber:iq:roster")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<DiscoInfoParser>("query", "http://jabber.org/protocol/disco#info")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<DiscoItemsParser>("query", "http://jabber.org/protocol/disco#items")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<CapsInfoParser>("c", "http://jabber.org/protocol/caps")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<ResourceBindParser>("bind", "urn:ietf:params:xml:ns:xmpp-bind")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StartSessionParser>("session", "urn:ietf:params:xml:ns:xmpp-session")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new SecurityLabelParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new SecurityLabelsCatalogParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new FormParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new CommandParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new InBandRegistrationPayloadParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new SearchPayloadParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new StreamInitiationParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new BytestreamsParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new VCardUpdateParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new VCardParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new PrivateStorageParserFactory(this)));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new ChatStateParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new DelayParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new MUCUserPayloadParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new NicknameParserFactory()));
	foreach(shared_ptr<PayloadParserFactory> factory, factories_) {
		addFactory(factory.get());
	}
	defaultFactory_ = new RawXMLPayloadParserFactory();
	setDefaultFactory(defaultFactory_);
}

FullPayloadParserFactoryCollection::~FullPayloadParserFactoryCollection() {
	setDefaultFactory(NULL);
	delete defaultFactory_;
	foreach(shared_ptr<PayloadParserFactory> factory, factories_) {
		removeFactory(factory.get());
	}
}

}
