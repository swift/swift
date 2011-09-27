/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/BlockPayload.h>
#include <Swiften/Elements/UnblockPayload.h>
#include <Swiften/Elements/BlockListPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/ErrorParser.h>
#include <Swiften/Parser/PayloadParsers/ErrorParserFactory.h>
#include <Swiften/Parser/PayloadParsers/BodyParser.h>
#include <Swiften/Parser/PayloadParsers/BlockParser.h>
#include <Swiften/Parser/PayloadParsers/SubjectParser.h>
#include <Swiften/Parser/PayloadParsers/ChatStateParserFactory.h>
#include <Swiften/Parser/PayloadParsers/PriorityParser.h>
#include <Swiften/Parser/PayloadParsers/ResourceBindParser.h>
#include <Swiften/Parser/PayloadParsers/StartSessionParser.h>
#include <Swiften/Parser/PayloadParsers/StatusParser.h>
#include <Swiften/Parser/PayloadParsers/StatusShowParser.h>
#include <Swiften/Parser/PayloadParsers/RosterItemExchangeParser.h>
#include <Swiften/Parser/PayloadParsers/RosterParser.h>
#include <Swiften/Parser/PayloadParsers/SoftwareVersionParser.h>
#include <Swiften/Parser/PayloadParsers/StorageParser.h>
#include <Swiften/Parser/PayloadParsers/DiscoInfoParser.h>
#include <Swiften/Parser/PayloadParsers/DiscoItemsParser.h>
#include <Swiften/Parser/PayloadParsers/CapsInfoParser.h>
#include <Swiften/Parser/PayloadParsers/SecurityLabelParserFactory.h>
#include <Swiften/Parser/PayloadParsers/SecurityLabelsCatalogParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParserFactory.h>
#include <Swiften/Parser/PayloadParsers/CommandParserFactory.h>
#include <Swiften/Parser/PayloadParsers/InBandRegistrationPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/SearchPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/StreamInitiationParserFactory.h>
#include <Swiften/Parser/PayloadParsers/BytestreamsParserFactory.h>
#include <Swiften/Parser/PayloadParsers/IBBParser.h>
#include <Swiften/Parser/PayloadParsers/VCardUpdateParserFactory.h>
#include <Swiften/Parser/PayloadParsers/VCardParserFactory.h>
#include <Swiften/Parser/PayloadParsers/RawXMLPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/PrivateStorageParserFactory.h>
#include <Swiften/Parser/PayloadParsers/DelayParser.h>
#include <Swiften/Parser/PayloadParsers/MUCUserPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MUCAdminPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/MUCOwnerPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/MUCOwnerPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/NicknameParserFactory.h>
#include <Swiften/Parser/PayloadParsers/ReplaceParser.h>
#include <Swiften/Parser/PayloadParsers/LastParser.h>
#include <Swiften/Parser/PayloadParsers/JingleParserFactory.h>
#include <Swiften/Parser/PayloadParsers/JingleReasonParser.h>
#include <Swiften/Parser/PayloadParsers/JingleContentPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/JingleIBBTransportMethodPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/JingleS5BTransportMethodPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/JingleFileTransferDescriptionParserFactory.h>
#include <Swiften/Parser/PayloadParsers/StreamInitiationFileInfoParser.h>
#include <Swiften/Parser/PayloadParsers/JingleFileTransferReceivedParser.h>
#include <Swiften/Parser/PayloadParsers/JingleFileTransferHashParser.h>
#include <Swiften/Parser/PayloadParsers/S5BProxyRequestParser.h>

#include "JingleIBBTransportMethodPayloadParser.h"
#include "JingleFileTransferDescriptionParser.h"

using namespace boost;

namespace Swift {

FullPayloadParserFactoryCollection::FullPayloadParserFactoryCollection() {
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<IBBParser>("", "http://jabber.org/protocol/ibb")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StatusShowParser>("show")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StatusParser>("status")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<ReplaceParser>("replace", "http://swift.im/protocol/replace")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<LastParser>("query", "jabber:iq:last")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<BodyParser>("body")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<SubjectParser>("subject")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<PriorityParser>("priority")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new ErrorParserFactory(this)));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<DelayParser>("delay", "urn:xmpp:delay")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<SoftwareVersionParser>("query", "jabber:iq:version")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StorageParser>("storage", "storage:bookmarks")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<RosterItemExchangeParser>("x", "http://jabber.org/protocol/rosterx")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<RosterParser>("query", "jabber:iq:roster")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<DiscoInfoParser>("query", "http://jabber.org/protocol/disco#info")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<DiscoItemsParser>("query", "http://jabber.org/protocol/disco#items")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<CapsInfoParser>("c", "http://jabber.org/protocol/caps")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<ResourceBindParser>("bind", "urn:ietf:params:xml:ns:xmpp-bind")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StartSessionParser>("session", "urn:ietf:params:xml:ns:xmpp-session")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<BlockParser<BlockPayload> >("block", "urn:xmpp:blocking")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<BlockParser<BlockListPayload> >("blocklist", "urn:xmpp:blocking")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<BlockParser<UnblockPayload> >("unblock", "urn:xmpp:blocking")));
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
	factories_.push_back(shared_ptr<PayloadParserFactory>(new MUCUserPayloadParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new MUCOwnerPayloadParserFactory(this)));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<MUCAdminPayloadParser>("query", "http://jabber.org/protocol/muc#admin")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new NicknameParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new JingleParserFactory(this)));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<JingleReasonParser>("reason", "urn:xmpp:jingle:1")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new JingleContentPayloadParserFactory(this)));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<JingleIBBTransportMethodPayloadParser>("transport", "urn:xmpp:jingle:transports:ibb:1")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<JingleS5BTransportMethodPayloadParser>("transport", "urn:xmpp:jingle:transports:s5b:1")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new JingleFileTransferDescriptionParserFactory(this)));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<StreamInitiationFileInfoParser>("file", "http://jabber.org/protocol/si/profile/file-transfer")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<JingleFileTransferReceivedParser>("received", "urn:xmpp:jingle:apps:file-transfer:3")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<JingleFileTransferHashParser>("checksum")));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new GenericPayloadParserFactory<S5BProxyRequestParser>("query", "http://jabber.org/protocol/bytestreams")));
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
