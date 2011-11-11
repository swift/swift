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
#include <Swiften/Parser/PayloadParsers/SecurityLabelsCatalogParser.h>
#include <Swiften/Parser/PayloadParsers/FormParserFactory.h>
#include <Swiften/Parser/PayloadParsers/CommandParser.h>
#include <Swiften/Parser/PayloadParsers/InBandRegistrationPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/SearchPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/StreamInitiationParser.h>
#include <Swiften/Parser/PayloadParsers/BytestreamsParser.h>
#include <Swiften/Parser/PayloadParsers/IBBParser.h>
#include <Swiften/Parser/PayloadParsers/VCardUpdateParser.h>
#include <Swiften/Parser/PayloadParsers/VCardParser.h>
#include <Swiften/Parser/PayloadParsers/RawXMLPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/PrivateStorageParserFactory.h>
#include <Swiften/Parser/PayloadParsers/DelayParser.h>
#include <Swiften/Parser/PayloadParsers/MUCUserPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MUCAdminPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/MUCOwnerPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/MUCDestroyPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/MUCInvitationPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/MUCOwnerPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/NicknameParser.h>
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
#include <Swiften/Parser/PayloadParsers/JingleIBBTransportMethodPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/JingleFileTransferDescriptionParser.h>
#include <Swiften/Parser/PayloadParsers/DeliveryReceiptParserFactory.h>
#include <Swiften/Parser/PayloadParsers/DeliveryReceiptRequestParserFactory.h>

using namespace boost;

namespace Swift {

FullPayloadParserFactoryCollection::FullPayloadParserFactoryCollection() {
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<IBBParser> >("", "http://jabber.org/protocol/ibb"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<StatusShowParser> >("show"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<StatusParser> >("status"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<ReplaceParser> >("replace", "http://swift.im/protocol/replace"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<LastParser> >("query", "jabber:iq:last"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<BodyParser> >("body"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<SubjectParser> >("subject"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<PriorityParser> >("priority"));
	factories_.push_back(boost::make_shared<ErrorParserFactory>(this));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<DelayParser> >("delay", "urn:xmpp:delay"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<SoftwareVersionParser> >("query", "jabber:iq:version"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<StorageParser> >("storage", "storage:bookmarks"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<RosterItemExchangeParser> >("x", "http://jabber.org/protocol/rosterx"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<RosterParser> >("query", "jabber:iq:roster"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<DiscoInfoParser> >("query", "http://jabber.org/protocol/disco#info"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<DiscoItemsParser> >("query", "http://jabber.org/protocol/disco#items"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<CapsInfoParser> >("c", "http://jabber.org/protocol/caps"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<ResourceBindParser> >("bind", "urn:ietf:params:xml:ns:xmpp-bind"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<StartSessionParser> >("session", "urn:ietf:params:xml:ns:xmpp-session"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<BlockParser<BlockPayload> > >("block", "urn:xmpp:blocking"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<BlockParser<BlockListPayload> > >("blocklist", "urn:xmpp:blocking"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<BlockParser<UnblockPayload> > >("unblock", "urn:xmpp:blocking"));
	factories_.push_back(boost::make_shared<SecurityLabelParserFactory>());
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<SecurityLabelsCatalogParser> >("catalog", "urn:xmpp:sec-label:catalog:2"));
	factories_.push_back(boost::make_shared<FormParserFactory>());
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<CommandParser> >("command", "http://jabber.org/protocol/commands"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<InBandRegistrationPayloadParser> >("query", "jabber:iq:register"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<SearchPayloadParser> >("query", "jabber:iq:search"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<StreamInitiationParser> >("si", "http://jabber.org/protocol/si"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<BytestreamsParser> >("query", "http://jabber.org/protocol/bytestreams"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<VCardUpdateParser> >("x", "vcard-temp:x:update"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<VCardParser> >("vCard", "vcard-temp"));
	factories_.push_back(boost::make_shared<PrivateStorageParserFactory>(this));
	factories_.push_back(boost::make_shared<ChatStateParserFactory>());
	factories_.push_back(boost::make_shared<MUCUserPayloadParserFactory>(this));
	factories_.push_back(boost::make_shared<MUCOwnerPayloadParserFactory>(this));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<MUCInvitationPayloadParser> >("x", "jabber:x:conference"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<MUCAdminPayloadParser> >("query", "http://jabber.org/protocol/muc#admin"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<MUCDestroyPayloadParser> >("destroy", "http://jabber.org/protocol/muc#user"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<MUCDestroyPayloadParser> >("destroy", "http://jabber.org/protocol/muc#owner"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<NicknameParser> >("nick", "http://jabber.org/protocol/nick"));
	factories_.push_back(boost::make_shared<JingleParserFactory>(this));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<JingleReasonParser> >("reason", "urn:xmpp:jingle:1"));
	factories_.push_back(boost::make_shared<JingleContentPayloadParserFactory>(this));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<JingleIBBTransportMethodPayloadParser> >("transport", "urn:xmpp:jingle:transports:ibb:1"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<JingleS5BTransportMethodPayloadParser> >("transport", "urn:xmpp:jingle:transports:s5b:1"));
	factories_.push_back(boost::make_shared<JingleFileTransferDescriptionParserFactory>(this));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<StreamInitiationFileInfoParser> >("file", "http://jabber.org/protocol/si/profile/file-transfer"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<JingleFileTransferReceivedParser> >("received", "urn:xmpp:jingle:apps:file-transfer:3"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<JingleFileTransferHashParser> >("checksum"));
	factories_.push_back(boost::make_shared<GenericPayloadParserFactory<S5BProxyRequestParser> >("query", "http://jabber.org/protocol/bytestreams"));
	factories_.push_back(boost::make_shared<DeliveryReceiptParserFactory>());
	factories_.push_back(boost::make_shared<DeliveryReceiptRequestParserFactory>());

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
