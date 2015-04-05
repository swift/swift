/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/algorithm/string.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <hippomocks.h>

#include <Swiften/Avatars/NullAvatarManager.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Disco/DummyEntityCapsProvider.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/MUC/MUCBookmarkManager.h>
#include <Swiften/MUC/UnitTest/MockMUC.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Presence/StanzaChannelPresenceSender.h>
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>

#include <Swift/Controllers/Chat/ChatMessageParser.h>
#include <Swift/Controllers/Chat/MUCController.h>
#include <Swift/Controllers/Chat/UserSearchController.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Settings/DummySettingsProvider.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindowFactory.h>
#include <Swift/Controllers/UnitTest/MockChatWindow.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

using namespace Swift;

class MUCControllerTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(MUCControllerTest);
	CPPUNIT_TEST(testJoinPartStringContructionSimple);
	CPPUNIT_TEST(testJoinPartStringContructionMixed);
	CPPUNIT_TEST(testAppendToJoinParts);
	CPPUNIT_TEST(testAddressedToSelf);
	CPPUNIT_TEST(testNotAddressedToSelf);
	CPPUNIT_TEST(testAddressedToSelfBySelf);
	CPPUNIT_TEST(testMessageWithEmptyLabelItem);
	CPPUNIT_TEST(testMessageWithLabelItem);
	CPPUNIT_TEST(testCorrectMessageWithLabelItem);
	CPPUNIT_TEST(testRoleAffiliationStates);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {
		crypto_ = boost::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
		self_ = JID("girl@wonderland.lit/rabbithole");
		nick_ = "aLiCe";
		mucJID_ = JID("teaparty@rooms.wonderland.lit");
		mocks_ = new MockRepository();
		stanzaChannel_ = new DummyStanzaChannel();
		iqChannel_ = new DummyIQChannel();
		iqRouter_ = new IQRouter(iqChannel_);
		eventController_ = new EventController();
		chatWindowFactory_ = mocks_->InterfaceMock<ChatWindowFactory>();
		userSearchWindowFactory_ = mocks_->InterfaceMock<UserSearchWindowFactory>();
		presenceOracle_ = new PresenceOracle(stanzaChannel_);
		presenceSender_ = new StanzaChannelPresenceSender(stanzaChannel_);
		directedPresenceSender_ = new DirectedPresenceSender(presenceSender_);
		uiEventStream_ = new UIEventStream();
		avatarManager_ = new NullAvatarManager();
		TimerFactory* timerFactory = NULL;
		window_ = new MockChatWindow();
		mucRegistry_ = new MUCRegistry();
		entityCapsProvider_ = new DummyEntityCapsProvider();
		settings_ = new DummySettingsProvider();
		highlightManager_ = new HighlightManager(settings_);
		muc_ = boost::make_shared<MockMUC>(mucJID_);
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(muc_->getJID(), uiEventStream_).Return(window_);
		chatMessageParser_ = boost::make_shared<ChatMessageParser>(std::map<std::string, std::string>(), highlightManager_->getRules(), true);
		vcardStorage_ = new VCardMemoryStorage(crypto_.get());
		vcardManager_ = new VCardManager(self_, iqRouter_, vcardStorage_);
		clientBlockListManager_ = new ClientBlockListManager(iqRouter_);
		mucBookmarkManager_ = new MUCBookmarkManager(iqRouter_);
		controller_ = new MUCController (self_, muc_, boost::optional<std::string>(), nick_, stanzaChannel_, iqRouter_, chatWindowFactory_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory, eventController_, entityCapsProvider_, NULL, NULL, mucRegistry_, highlightManager_, clientBlockListManager_, chatMessageParser_, false, NULL, vcardManager_, mucBookmarkManager_);
	}

	void tearDown() {
		delete controller_;
		delete mucBookmarkManager_;
		delete clientBlockListManager_;
		delete vcardManager_;
		delete vcardStorage_;
		delete highlightManager_;
		delete settings_;
		delete entityCapsProvider_;
		delete eventController_;
		delete presenceOracle_;
		delete mocks_;
		delete uiEventStream_;
		delete stanzaChannel_;
		delete presenceSender_;
		delete directedPresenceSender_;
		delete iqRouter_;
		delete iqChannel_;
		delete mucRegistry_;
		delete avatarManager_;
	}

	void finishJoin() {
		Presence::ref presence(new Presence());
		presence->setFrom(JID(muc_->getJID().toString() + "/" + nick_));
		MUCUserPayload::ref status(new MUCUserPayload());
		MUCUserPayload::StatusCode code;
		code.code = 110;
		status->addStatusCode(code);
		presence->addPayload(status);
		stanzaChannel_->onPresenceReceived(presence);
	}

	void testAddressedToSelf() {
		finishJoin();
		Message::ref message(new Message());

		message = Message::ref(new Message());
		message->setFrom(JID(muc_->getJID().toString() + "/otherperson"));
		message->setBody("basic " + nick_ + " test.");
		message->setType(Message::Groupchat);
		controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
		CPPUNIT_ASSERT_EQUAL((size_t)1, eventController_->getEvents().size());

		message = Message::ref(new Message());
		message->setFrom(JID(muc_->getJID().toString() + "/otherperson"));
		message->setBody(nick_ + ": hi there");
		message->setType(Message::Groupchat);
		controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
		CPPUNIT_ASSERT_EQUAL((size_t)2, eventController_->getEvents().size());

		message->setFrom(JID(muc_->getJID().toString() + "/other"));
		message->setBody("Hi there " + nick_);
		message->setType(Message::Groupchat);
		controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
		CPPUNIT_ASSERT_EQUAL((size_t)3, eventController_->getEvents().size());

		message = Message::ref(new Message());
		message->setFrom(JID(muc_->getJID().toString() + "/other2"));
		message->setBody("Hi " + boost::to_lower_copy(nick_) + ".");
		message->setType(Message::Groupchat);
		controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
		CPPUNIT_ASSERT_EQUAL((size_t)4, eventController_->getEvents().size());

		message = Message::ref(new Message());
		message->setFrom(JID(muc_->getJID().toString() + "/other3"));
		message->setBody("Hi bert.");
		message->setType(Message::Groupchat);
		controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
		CPPUNIT_ASSERT_EQUAL((size_t)4, eventController_->getEvents().size());

		message = Message::ref(new Message());
		message->setFrom(JID(muc_->getJID().toString() + "/other2"));
		message->setBody("Hi " + boost::to_lower_copy(nick_) + "ie.");
		message->setType(Message::Groupchat);
		controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
		CPPUNIT_ASSERT_EQUAL((size_t)4, eventController_->getEvents().size());
	}

	void testNotAddressedToSelf() {
		finishJoin();
		Message::ref message(new Message());
		message->setFrom(JID(muc_->getJID().toString() + "/other3"));
		message->setBody("Hi there Hatter");
		message->setType(Message::Groupchat);
		controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
		CPPUNIT_ASSERT_EQUAL((size_t)0, eventController_->getEvents().size());
	}

	void testAddressedToSelfBySelf() {
		finishJoin();
		Message::ref message(new Message());
		message->setFrom(JID(muc_->getJID().toString() + "/" + nick_));
		message->setBody("Hi there " + nick_);
		message->setType(Message::Groupchat);
		controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
		CPPUNIT_ASSERT_EQUAL((size_t)0, eventController_->getEvents().size());
	}

	void testMessageWithEmptyLabelItem() {
		SecurityLabelsCatalog::Item label;
		label.setSelector("Bob");
		window_->label_ = label;
		boost::shared_ptr<DiscoInfo> features = boost::make_shared<DiscoInfo>();
		features->addFeature(DiscoInfo::SecurityLabelsCatalogFeature);
		controller_->setAvailableServerFeatures(features);
		IQ::ref iq = iqChannel_->iqs_[iqChannel_->iqs_.size() - 1];
		SecurityLabelsCatalog::ref labelPayload = boost::make_shared<SecurityLabelsCatalog>();
		labelPayload->addItem(label);
		IQ::ref result = IQ::createResult(self_, iq->getID(), labelPayload);
		iqChannel_->onIQReceived(result);
		std::string messageBody("agamemnon");
		window_->onSendMessageRequest(messageBody, false);
		boost::shared_ptr<Stanza> rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
		Message::ref message = boost::dynamic_pointer_cast<Message>(rawStanza);
		CPPUNIT_ASSERT_EQUAL(iq->getTo(), result->getFrom());
		CPPUNIT_ASSERT(window_->labelsEnabled_);
		CPPUNIT_ASSERT(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
		CPPUNIT_ASSERT(message);
		CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody());
		CPPUNIT_ASSERT(!message->getPayload<SecurityLabel>());
	}

	void testMessageWithLabelItem() {
		boost::shared_ptr<SecurityLabel> label = boost::make_shared<SecurityLabel>();
		label->setLabel("a");
		SecurityLabelsCatalog::Item labelItem;
		labelItem.setSelector("Bob");
		labelItem.setLabel(label);
		window_->label_ = labelItem;
		boost::shared_ptr<DiscoInfo> features = boost::make_shared<DiscoInfo>();
		features->addFeature(DiscoInfo::SecurityLabelsCatalogFeature);
		controller_->setAvailableServerFeatures(features);
		IQ::ref iq = iqChannel_->iqs_[iqChannel_->iqs_.size() - 1];
		SecurityLabelsCatalog::ref labelPayload = boost::make_shared<SecurityLabelsCatalog>();
		labelPayload->addItem(labelItem);
		IQ::ref result = IQ::createResult(self_, iq->getID(), labelPayload);
		iqChannel_->onIQReceived(result);
		std::string messageBody("agamemnon");
		window_->onSendMessageRequest(messageBody, false);
		boost::shared_ptr<Stanza> rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
		Message::ref message = boost::dynamic_pointer_cast<Message>(rawStanza);
		CPPUNIT_ASSERT_EQUAL(iq->getTo(), result->getFrom());
		CPPUNIT_ASSERT(window_->labelsEnabled_);
		CPPUNIT_ASSERT(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
		CPPUNIT_ASSERT(message);
		CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody());
		CPPUNIT_ASSERT_EQUAL(label, message->getPayload<SecurityLabel>());
	}

	void testCorrectMessageWithLabelItem() {
		boost::shared_ptr<SecurityLabel> label = boost::make_shared<SecurityLabel>();
		label->setLabel("a");
		SecurityLabelsCatalog::Item labelItem;
		labelItem.setSelector("Bob");
		labelItem.setLabel(label);
		boost::shared_ptr<SecurityLabel> label2 = boost::make_shared<SecurityLabel>();
		label->setLabel("b");
		SecurityLabelsCatalog::Item labelItem2;
		labelItem2.setSelector("Charlie");
		labelItem2.setLabel(label2);
		window_->label_ = labelItem;
		boost::shared_ptr<DiscoInfo> features = boost::make_shared<DiscoInfo>();
		features->addFeature(DiscoInfo::SecurityLabelsCatalogFeature);
		controller_->setAvailableServerFeatures(features);
		IQ::ref iq = iqChannel_->iqs_[iqChannel_->iqs_.size() - 1];
		SecurityLabelsCatalog::ref labelPayload = boost::make_shared<SecurityLabelsCatalog>();
		labelPayload->addItem(labelItem);
		IQ::ref result = IQ::createResult(self_, iq->getID(), labelPayload);
		iqChannel_->onIQReceived(result);
		std::string messageBody("agamemnon");
		window_->onSendMessageRequest(messageBody, false);
		boost::shared_ptr<Stanza> rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
		Message::ref message = boost::dynamic_pointer_cast<Message>(rawStanza);
		CPPUNIT_ASSERT_EQUAL(iq->getTo(), result->getFrom());
		CPPUNIT_ASSERT(window_->labelsEnabled_);
		CPPUNIT_ASSERT(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
		CPPUNIT_ASSERT(message);
		CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody());
		CPPUNIT_ASSERT_EQUAL(label, message->getPayload<SecurityLabel>());
		window_->label_ = labelItem2;
		window_->onSendMessageRequest(messageBody, true);
		rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
		message = boost::dynamic_pointer_cast<Message>(rawStanza);
		CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody());
		CPPUNIT_ASSERT_EQUAL(label, message->getPayload<SecurityLabel>());
	}

	void checkEqual(const std::vector<NickJoinPart>& expected, const std::vector<NickJoinPart>& actual) {
		CPPUNIT_ASSERT_EQUAL(expected.size(), actual.size());
		for (size_t i = 0; i < expected.size(); i++) {
			CPPUNIT_ASSERT_EQUAL(expected[i].nick, actual[i].nick);
			CPPUNIT_ASSERT_EQUAL(expected[i].type, actual[i].type);
		}
	}

	void testAppendToJoinParts() {
		std::vector<NickJoinPart> list;
		std::vector<NickJoinPart> gold;
		MUCController::appendToJoinParts(list, NickJoinPart("Kev", Join));
		gold.push_back(NickJoinPart("Kev", Join));
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Remko", Join));
		gold.push_back(NickJoinPart("Remko", Join));
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Bert", Join));
		gold.push_back(NickJoinPart("Bert", Join));
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Bert", Part));
		gold[2].type = JoinThenPart;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Kev", Part));
		gold[0].type = JoinThenPart;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Remko", Part));
		gold[1].type = JoinThenPart;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Ernie", Part));
		gold.push_back(NickJoinPart("Ernie", Part));
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Ernie", Join));
		gold[3].type = PartThenJoin;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Kev", Join));
		gold[0].type = Join;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Ernie", Part));
		gold[3].type = Part;
		checkEqual(gold, list);

	}

	void testJoinPartStringContructionSimple() {
		std::vector<NickJoinPart> list;
		list.push_back(NickJoinPart("Kev", Join));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev has entered the room"), MUCController::generateJoinPartString(list, false));
		list.push_back(NickJoinPart("Remko", Part));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev has entered the room and Remko has left the room"), MUCController::generateJoinPartString(list, false));
		list.push_back(NickJoinPart("Bert", Join));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev and Bert have entered the room and Remko has left the room"), MUCController::generateJoinPartString(list, false));
		list.push_back(NickJoinPart("Ernie", Join));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev, Bert and Ernie have entered the room and Remko has left the room"), MUCController::generateJoinPartString(list, false));
	}

	void testJoinPartStringContructionMixed() {
		std::vector<NickJoinPart> list;
		list.push_back(NickJoinPart("Kev", JoinThenPart));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev has entered then left the room"), MUCController::generateJoinPartString(list, false));
		list.push_back(NickJoinPart("Remko", Part));
		CPPUNIT_ASSERT_EQUAL(std::string("Remko has left the room and Kev has entered then left the room"), MUCController::generateJoinPartString(list, false));
		list.push_back(NickJoinPart("Bert", PartThenJoin));
		CPPUNIT_ASSERT_EQUAL(std::string("Remko has left the room, Kev has entered then left the room and Bert has left then returned to the room"), MUCController::generateJoinPartString(list, false));
		list.push_back(NickJoinPart("Ernie", JoinThenPart));
		CPPUNIT_ASSERT_EQUAL(std::string("Remko has left the room, Kev and Ernie have entered then left the room and Bert has left then returned to the room"), MUCController::generateJoinPartString(list, false));
	}

	JID jidFromOccupant(const MUCOccupant& occupant) {
		return JID(mucJID_.toString()+"/"+occupant.getNick());
	}

	void testRoleAffiliationStates() {

		typedef std::map<std::string, MUCOccupant> occupant_map;
		occupant_map occupants;
		occupants.insert(occupant_map::value_type("Kev", MUCOccupant("Kev", MUCOccupant::Participant, MUCOccupant::Owner)));
		occupants.insert(occupant_map::value_type("Remko", MUCOccupant("Remko", MUCOccupant::Participant, MUCOccupant::Owner)));
		occupants.insert(occupant_map::value_type("Bert", MUCOccupant("Bert", MUCOccupant::Participant, MUCOccupant::Owner)));
		occupants.insert(occupant_map::value_type("Ernie", MUCOccupant("Ernie", MUCOccupant::Participant, MUCOccupant::Owner)));

		/* populate the MUC with fake users */
		typedef const std::pair<std::string,MUCOccupant> occupantIterator;
		foreach(occupantIterator &occupant, occupants) {
			muc_->insertOccupant(occupant.second);
		}

		std::vector<MUCOccupant> alterations;
		alterations.push_back(MUCOccupant("Kev", MUCOccupant::Visitor, MUCOccupant::Admin));
		alterations.push_back(MUCOccupant("Remko", MUCOccupant::Moderator, MUCOccupant::Member));
		alterations.push_back(MUCOccupant("Bert", MUCOccupant::Visitor, MUCOccupant::Outcast));
		alterations.push_back(MUCOccupant("Ernie", MUCOccupant::NoRole, MUCOccupant::Member));
		alterations.push_back(MUCOccupant("Bert", MUCOccupant::Moderator, MUCOccupant::Owner));
		alterations.push_back(MUCOccupant("Kev", MUCOccupant::Participant, MUCOccupant::Outcast));
		alterations.push_back(MUCOccupant("Bert", MUCOccupant::Visitor, MUCOccupant::NoAffiliation));
		alterations.push_back(MUCOccupant("Remko", MUCOccupant::NoRole, MUCOccupant::NoAffiliation));
		alterations.push_back(MUCOccupant("Ernie", MUCOccupant::Visitor, MUCOccupant::Outcast));

		foreach(const MUCOccupant& alteration, alterations) {
			/* perform an alteration to a user's role and affiliation */
			occupant_map::iterator occupant = occupants.find(alteration.getNick());
			CPPUNIT_ASSERT(occupant != occupants.end());
			const JID jid = jidFromOccupant(occupant->second);
			/* change the affiliation, leave the role in place */
			muc_->changeAffiliation(jid, alteration.getAffiliation());
			occupant->second = MUCOccupant(occupant->first, occupant->second.getRole(), alteration.getAffiliation());
			testRoleAffiliationStatesVerify(occupants);
			/* change the role, leave the affiliation in place */
			muc_->changeOccupantRole(jid, alteration.getRole());
			occupant->second = MUCOccupant(occupant->first, alteration.getRole(), occupant->second.getAffiliation());
			testRoleAffiliationStatesVerify(occupants);
		}
	}

	void testRoleAffiliationStatesVerify(const std::map<std::string, MUCOccupant> &occupants) {
		/* verify that the roster is in sync */
		GroupRosterItem* group = window_->getRosterModel()->getRoot();
		foreach(RosterItem* rosterItem, group->getChildren()) {
			GroupRosterItem* child = dynamic_cast<GroupRosterItem*>(rosterItem);
			CPPUNIT_ASSERT(child);
			foreach(RosterItem* childItem, child->getChildren()) {
				ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(childItem);
				CPPUNIT_ASSERT(item);
				std::map<std::string, MUCOccupant>::const_iterator occupant = occupants.find(item->getJID().getResource());
				CPPUNIT_ASSERT(occupant != occupants.end());
				CPPUNIT_ASSERT(item->getMUCRole() == occupant->second.getRole());
				CPPUNIT_ASSERT(item->getMUCAffiliation() == occupant->second.getAffiliation());
			}
		}
	}

private:
	JID self_;
	JID mucJID_;
	MockMUC::ref muc_;
	std::string nick_;
	DummyStanzaChannel* stanzaChannel_;
	DummyIQChannel* iqChannel_;
	IQRouter* iqRouter_;
	EventController* eventController_;
	ChatWindowFactory* chatWindowFactory_;
	UserSearchWindowFactory* userSearchWindowFactory_;
	MUCController* controller_;
//	NickResolver* nickResolver_;
	PresenceOracle* presenceOracle_;
	AvatarManager* avatarManager_;
	StanzaChannelPresenceSender* presenceSender_;
	DirectedPresenceSender* directedPresenceSender_;
	MockRepository* mocks_;
	UIEventStream* uiEventStream_;
	MockChatWindow* window_;
	MUCRegistry* mucRegistry_;
	DummyEntityCapsProvider* entityCapsProvider_;
	DummySettingsProvider* settings_;
	HighlightManager* highlightManager_;
	boost::shared_ptr<ChatMessageParser> chatMessageParser_;
	boost::shared_ptr<CryptoProvider> crypto_;
	VCardManager* vcardManager_;
	VCardMemoryStorage* vcardStorage_;
	ClientBlockListManager* clientBlockListManager_;
	MUCBookmarkManager* mucBookmarkManager_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCControllerTest);

