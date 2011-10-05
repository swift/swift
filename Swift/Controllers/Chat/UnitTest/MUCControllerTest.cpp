/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/algorithm/string.hpp>
#include "3rdParty/hippomocks.h"

#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swiften/Presence/DirectedPresenceSender.h"
#include "Swiften/Presence/StanzaChannelPresenceSender.h"
#include "Swiften/Avatars/NullAvatarManager.h"
#include "Swift/Controllers/Chat/MUCController.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swiften/Client/NickResolver.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UnitTest/MockChatWindow.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Network/TimerFactory.h"
#include "Swiften/Elements/MUCUserPayload.h"
#include "Swiften/Disco/DummyEntityCapsProvider.h"

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
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {
		self_ = JID("girl@wonderland.lit/rabbithole");
		nick_ = "aLiCe";
		mucJID_ = JID("teaparty@rooms.wonderland.lit");
		mocks_ = new MockRepository();
		stanzaChannel_ = new DummyStanzaChannel();
		iqChannel_ = new DummyIQChannel();
		iqRouter_ = new IQRouter(iqChannel_);
		eventController_ = new EventController();
		chatWindowFactory_ = mocks_->InterfaceMock<ChatWindowFactory>();
		presenceOracle_ = new PresenceOracle(stanzaChannel_);
		presenceSender_ = new StanzaChannelPresenceSender(stanzaChannel_);
		directedPresenceSender_ = new DirectedPresenceSender(presenceSender_);
		uiEventStream_ = new UIEventStream();
		avatarManager_ = new NullAvatarManager();
		TimerFactory* timerFactory = NULL;
		window_ = new MockChatWindow();
		mucRegistry_ = new MUCRegistry();
		entityCapsProvider_ = new DummyEntityCapsProvider();
		muc_ = boost::make_shared<MUC>(stanzaChannel_, iqRouter_, directedPresenceSender_, mucJID_, mucRegistry_);
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(muc_->getJID(), uiEventStream_).Return(window_);
		controller_ = new MUCController (self_, muc_, boost::optional<std::string>(), nick_, stanzaChannel_, iqRouter_, chatWindowFactory_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory, eventController_, entityCapsProvider_);
	};

	void tearDown() {
		delete entityCapsProvider_;
		delete controller_;
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
		SecurityLabel::ref label = boost::make_shared<SecurityLabel>();
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
		CPPUNIT_ASSERT_EQUAL(std::string("Kev has entered the room"), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Remko", Part));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev has entered the room and Remko has left the room"), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Bert", Join));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev and Bert have entered the room and Remko has left the room"), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Ernie", Join));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev, Bert and Ernie have entered the room and Remko has left the room"), MUCController::generateJoinPartString(list));
	}

	void testJoinPartStringContructionMixed() {
		std::vector<NickJoinPart> list;
		list.push_back(NickJoinPart("Kev", JoinThenPart));
		CPPUNIT_ASSERT_EQUAL(std::string("Kev has entered then left the room"), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Remko", Part));
		CPPUNIT_ASSERT_EQUAL(std::string("Remko has left the room and Kev has entered then left the room"), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Bert", PartThenJoin));
		CPPUNIT_ASSERT_EQUAL(std::string("Remko has left the room, Kev has entered then left the room and Bert has left then returned to the room"), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Ernie", JoinThenPart));
		CPPUNIT_ASSERT_EQUAL(std::string("Remko has left the room, Kev and Ernie have entered then left the room and Bert has left then returned to the room"), MUCController::generateJoinPartString(list));
	}

private:
	JID self_;
	JID mucJID_;
	MUC::ref muc_;
	std::string nick_;
	DummyStanzaChannel* stanzaChannel_;
	DummyIQChannel* iqChannel_;
	IQRouter* iqRouter_;
	EventController* eventController_;
	ChatWindowFactory* chatWindowFactory_;
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
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCControllerTest);

