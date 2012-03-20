/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include "3rdParty/hippomocks.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/Chat/ChatsManager.h"

#include "Swift/Controllers/Chat/UnitTest/MockChatListWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/Settings/DummySettingsProvider.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/ChatListWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/JoinMUCWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h"
#include "Swiften/Client/Client.h"
#include "Swiften/Disco/EntityCapsManager.h"
#include "Swiften/Disco/CapsProvider.h"
#include "Swiften/MUC/MUCManager.h"
#include "Swift/Controllers/Chat/ChatController.h"
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swift/Controllers/Chat/MUCController.h"
#include "Swiften/Presence/StanzaChannelPresenceSender.h"
#include "Swiften/Avatars/NullAvatarManager.h"
#include "Swiften/Avatars/AvatarMemoryStorage.h"
#include "Swiften/VCards/VCardManager.h"
#include "Swiften/VCards/VCardMemoryStorage.h"
#include "Swiften/Client/NickResolver.h"
#include "Swiften/Presence/DirectedPresenceSender.h"
#include "Swiften/Roster/XMPPRosterImpl.h"
#include "Swift/Controllers/UnitTest/MockChatWindow.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Jingle/JingleSessionManager.h"
#include "Swiften/FileTransfer/UnitTest/DummyFileTransferManager.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include <Swift/Controllers/ProfileSettingsProvider.h>
#include "Swift/Controllers/FileTransfer/FileTransferOverview.h"
#include "Swiften/Elements/DeliveryReceiptRequest.h"
#include "Swiften/Elements/DeliveryReceipt.h"
#include <Swiften/Base/Algorithm.h>
#include <Swift/Controllers/SettingConstants.h>

using namespace Swift;

class DummyCapsProvider : public CapsProvider {
		DiscoInfo::ref getCaps(const std::string&) const {return DiscoInfo::ref(new DiscoInfo());}
};

class ChatsManagerTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ChatsManagerTest);
	CPPUNIT_TEST(testFirstOpenWindowIncoming);
	CPPUNIT_TEST(testSecondOpenWindowIncoming);
	CPPUNIT_TEST(testFirstOpenWindowOutgoing);
	CPPUNIT_TEST(testFirstOpenWindowBareToFull);
	CPPUNIT_TEST(testSecondWindow);
	CPPUNIT_TEST(testUnbindRebind);
	CPPUNIT_TEST(testNoDuplicateUnbind);
	CPPUNIT_TEST(testThreeMUCWindows);
	CPPUNIT_TEST(testChatControllerPresenceAccessUpdatedOnRemoveFromRoster);
	CPPUNIT_TEST(testChatControllerPresenceAccessUpdatedOnAddToRoster);
	CPPUNIT_TEST(testChatControllerPresenceAccessUpdatedOnSubscriptionChangeToBoth);
	CPPUNIT_TEST(testChatControllerPresenceAccessUpdatedOnSubscriptionChangeToFrom);
	CPPUNIT_TEST_SUITE_END();
	
public:
	void setUp() { 
		mocks_ = new MockRepository();
		jid_ = JID("test@test.com/resource");
		stanzaChannel_ = new DummyStanzaChannel();
		iqChannel_ = new DummyIQChannel();
		iqRouter_ = new IQRouter(iqChannel_);
		capsProvider_ = new DummyCapsProvider();
		eventController_ = new EventController();
		chatWindowFactory_ = mocks_->InterfaceMock<ChatWindowFactory>();
		joinMUCWindowFactory_ = mocks_->InterfaceMock<JoinMUCWindowFactory>();
		xmppRoster_ = new XMPPRosterImpl();
		mucRegistry_ = new MUCRegistry();
		nickResolver_ = new NickResolver(jid_.toBare(), xmppRoster_, NULL, mucRegistry_);
		presenceOracle_ = new PresenceOracle(stanzaChannel_);
		serverDiscoInfo_ = boost::make_shared<DiscoInfo>();
		presenceSender_ = new StanzaChannelPresenceSender(stanzaChannel_);
		directedPresenceSender_ = new DirectedPresenceSender(presenceSender_);
		mucManager_ = new MUCManager(stanzaChannel_, iqRouter_, directedPresenceSender_, mucRegistry_);
		uiEventStream_ = new UIEventStream();
		entityCapsManager_ = new EntityCapsManager(capsProvider_, stanzaChannel_);
		chatListWindowFactory_ = mocks_->InterfaceMock<ChatListWindowFactory>();
		mucSearchWindowFactory_ = mocks_->InterfaceMock<MUCSearchWindowFactory>();
		settings_ = new DummySettingsProvider();
		profileSettings_ = new ProfileSettingsProvider("a", settings_);
		chatListWindow_ = new MockChatListWindow();
		ftManager_ = new DummyFileTransferManager();
		ftOverview_ = new FileTransferOverview(ftManager_);

		mocks_->ExpectCall(chatListWindowFactory_, ChatListWindowFactory::createChatListWindow).With(uiEventStream_).Return(chatListWindow_);
		manager_ = new ChatsManager(jid_, stanzaChannel_, iqRouter_, eventController_, chatWindowFactory_, joinMUCWindowFactory_, nickResolver_, presenceOracle_, directedPresenceSender_, uiEventStream_, chatListWindowFactory_, true, NULL, mucRegistry_, entityCapsManager_, mucManager_, mucSearchWindowFactory_, profileSettings_, ftOverview_, xmppRoster_, false, settings_);

		avatarManager_ = new NullAvatarManager();
		manager_->setAvatarManager(avatarManager_);
	};
	
	void tearDown() {
		//delete chatListWindowFactory
		delete profileSettings_;
		delete avatarManager_;
		delete manager_;
		delete ftOverview_;
		delete ftManager_;
		delete directedPresenceSender_;
		delete presenceSender_;
		delete presenceOracle_;
		delete nickResolver_;
		delete mucRegistry_;
		delete stanzaChannel_;
		delete eventController_;
		delete iqRouter_;
		delete iqChannel_;
		delete uiEventStream_;
		delete mucManager_;
		delete xmppRoster_;
		delete entityCapsManager_;
		delete capsProvider_;
		delete chatListWindow_;
		delete mocks_;
		delete settings_;
	}

	void testFirstOpenWindowIncoming() {
		JID messageJID("testling@test.com/resource1");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

		boost::shared_ptr<Message> message(new Message());
		message->setFrom(messageJID);
		std::string body("This is a legible message. >HEH@)oeueu");
		message->setBody(body);
		manager_->handleIncomingMessage(message);
		CPPUNIT_ASSERT_EQUAL(body, window->lastMessageBody_);
	}

	void testSecondOpenWindowIncoming() {
		JID messageJID1("testling@test.com/resource1");
		
		MockChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID1, uiEventStream_).Return(window1);

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(messageJID1);
		std::string body1("This is a legible message. >HEH@)oeueu");
		message1->setBody(body1);
		manager_->handleIncomingMessage(message1);
		CPPUNIT_ASSERT_EQUAL(body1, window1->lastMessageBody_);

		JID messageJID2("testling@test.com/resource2");
		
		//MockChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		//mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID2, uiEventStream_).Return(window2);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(messageJID2);
		std::string body2("This is a legible message. .cmaulm.chul");
		message2->setBody(body2);
		manager_->handleIncomingMessage(message2);
		CPPUNIT_ASSERT_EQUAL(body2, window1->lastMessageBody_);
	}

	void testFirstOpenWindowOutgoing() {
		std::string messageJIDString("testling@test.com");
		
		ChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString), uiEventStream_).Return(window);
		
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString))));
	}


	void testFirstOpenWindowBareToFull() {
		std::string bareJIDString("testling@test.com");
		std::string fullJIDString("testling@test.com/resource1");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString), uiEventStream_).Return(window);
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(bareJIDString))));

		boost::shared_ptr<Message> message(new Message());
		message->setFrom(JID(fullJIDString));
		std::string body("This is a legible message. mjuga3089gm8G(*>M)@*(");
		message->setBody(body);
		manager_->handleIncomingMessage(message);
		CPPUNIT_ASSERT_EQUAL(body, window->lastMessageBody_);
	}

	void testSecondWindow() {
		std::string messageJIDString1("testling1@test.com");
		ChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString1), uiEventStream_).Return(window1);
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString1))));

		std::string messageJIDString2("testling2@test.com");
		ChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString2), uiEventStream_).Return(window2);

		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString2))));
	}

	/** Complete cycle.
		Create unbound window.
		Bind it.
		Unbind it.
		Rebind it.
	 */
	void testUnbindRebind() {
		std::string bareJIDString("testling@test.com");
		std::string fullJIDString1("testling@test.com/resource1");
		std::string fullJIDString2("testling@test.com/resource2");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString), uiEventStream_).Return(window);
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(bareJIDString))));

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(JID(fullJIDString1));
		std::string messageBody1("This is a legible message.");
		message1->setBody(messageBody1);
		manager_->handleIncomingMessage(message1);
		CPPUNIT_ASSERT_EQUAL(messageBody1, window->lastMessageBody_);
		
		boost::shared_ptr<Presence> jid1Online(new Presence());
		jid1Online->setFrom(JID(fullJIDString1));
		boost::shared_ptr<Presence> jid1Offline(new Presence());
		jid1Offline->setFrom(JID(fullJIDString1));
		jid1Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid1Offline);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(JID(fullJIDString2));
		std::string messageBody2("This is another legible message.");
		message2->setBody(messageBody2);
		manager_->handleIncomingMessage(message2);
		CPPUNIT_ASSERT_EQUAL(messageBody2, window->lastMessageBody_);
	}

	/**
	 * Test that MUC PMs get opened in the right windows
	 */
	void testThreeMUCWindows() {
		JID muc("testling@test.com");
		ChatWindow* mucWindow = new MockChatWindow();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(muc, uiEventStream_).Return(mucWindow);
		uiEventStream_->send(boost::make_shared<JoinMUCUIEvent>(muc, std::string("nick")));


		std::string messageJIDString1("testling@test.com/1");
		ChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString1), uiEventStream_).Return(window1);
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString1))));

		std::string messageJIDString2("testling@test.com/2");
		ChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString2), uiEventStream_).Return(window2);

		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString2))));

		std::string messageJIDString3("testling@test.com/3");
		ChatWindow* window3 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString3), uiEventStream_).Return(window3);

		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString3))));

		/* Refetch an earlier window */
		/* We do not expect a new window to be created */
		uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(JID(messageJIDString1))));

	}

	/**
		 Test that a second window isn't unbound where there's already an unbound one.
		 Bind 1
		 Bind 2
		 Unbind 1
		 Unbind 2 (but it doesn't)
		 Sent to bound 2
		 Rebind 1
	 */
	void testNoDuplicateUnbind() {
		JID messageJID1("testling@test.com/resource1");
		
		MockChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID1, uiEventStream_).Return(window1);

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(messageJID1);
		message1->setBody("This is a legible message1.");
		manager_->handleIncomingMessage(message1);

		JID messageJID2("testling@test.com/resource2");
		
		//MockChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		//mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID2, uiEventStream_).Return(window2);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(messageJID2);
		message2->setBody("This is a legible message2.");
		manager_->handleIncomingMessage(message2);

		boost::shared_ptr<Presence> jid1Online(new Presence());
		jid1Online->setFrom(JID(messageJID1));
		boost::shared_ptr<Presence> jid1Offline(new Presence());
		jid1Offline->setFrom(JID(messageJID1));
		jid1Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid1Offline);
 
		boost::shared_ptr<Presence> jid2Online(new Presence());
		jid2Online->setFrom(JID(messageJID2));
		boost::shared_ptr<Presence> jid2Offline(new Presence());
		jid2Offline->setFrom(JID(messageJID2));
		jid2Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid2Offline);

		JID messageJID3("testling@test.com/resource3");

		boost::shared_ptr<Message> message3(new Message());
		message3->setFrom(messageJID3);
		std::string body3("This is a legible message3.");
		message3->setBody(body3);
		manager_->handleIncomingMessage(message3);
		CPPUNIT_ASSERT_EQUAL(body3, window1->lastMessageBody_);

		boost::shared_ptr<Message> message2b(new Message());
		message2b->setFrom(messageJID2);
		std::string body2b("This is a legible message2b.");
		message2b->setBody(body2b);
		manager_->handleIncomingMessage(message2b);
		CPPUNIT_ASSERT_EQUAL(body2b, window1->lastMessageBody_);
	}

	/**
	 *	Test that ChatController doesn't send receipts anymore after removal of the contact from the roster.
	 */
	void testChatControllerPresenceAccessUpdatedOnRemoveFromRoster() {
		JID messageJID("testling@test.com/resource1");
		xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), RosterItemPayload::Both);

		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);
		settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

		boost::shared_ptr<Message> message = makeDeliveryReceiptTestMessage(messageJID, "1");
		manager_->handleIncomingMessage(message);
		Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(0);
		CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->sentStanzas.size());
		CPPUNIT_ASSERT(stanzaContactOnRoster->getPayload<DeliveryReceipt>() != 0);

		xmppRoster_->removeContact(messageJID);

		message->setID("2");
		manager_->handleIncomingMessage(message);
		CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->sentStanzas.size());
	}

	/**
	 *	Test that ChatController sends receipts after the contact has been added to the roster.
	 */
	void testChatControllerPresenceAccessUpdatedOnAddToRoster() {
		JID messageJID("testling@test.com/resource1");

		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);
		settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

		boost::shared_ptr<Message> message = makeDeliveryReceiptTestMessage(messageJID, "1");
		manager_->handleIncomingMessage(message);

		CPPUNIT_ASSERT_EQUAL(st(0), stanzaChannel_->sentStanzas.size());

		xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), RosterItemPayload::Both);
		message->setID("2");
		manager_->handleIncomingMessage(message);

		CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->sentStanzas.size());
		Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(0);
		CPPUNIT_ASSERT(stanzaContactOnRoster->getPayload<DeliveryReceipt>() != 0);
	}

	/**
	 *	Test that ChatController sends receipts if requested after change from subscription state To to subscription state Both.
	 */
	void testChatControllerPresenceAccessUpdatedOnSubscriptionChangeToBoth() {
		testhelperChatControllerPresenceAccessUpdatedOnSubscriptionChangeReceiptsAllowed(RosterItemPayload::To, RosterItemPayload::Both);
	}

	/**
	 *	Test that ChatController sends receipts if requested after change from subscription state To to subscription state From.
	 */
	void testChatControllerPresenceAccessUpdatedOnSubscriptionChangeToFrom() {
		testhelperChatControllerPresenceAccessUpdatedOnSubscriptionChangeReceiptsAllowed(RosterItemPayload::To, RosterItemPayload::From);
	}

	void testhelperChatControllerPresenceAccessUpdatedOnSubscriptionChangeReceiptsAllowed(RosterItemPayload::Subscription from, RosterItemPayload::Subscription to) {
		JID messageJID("testling@test.com/resource1");
		xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), from);

		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);
		settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

		boost::shared_ptr<Message> message = makeDeliveryReceiptTestMessage(messageJID, "1");
		manager_->handleIncomingMessage(message);

		CPPUNIT_ASSERT_EQUAL(st(0), stanzaChannel_->sentStanzas.size());

		xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), to);
		message->setID("2");
		manager_->handleIncomingMessage(message);

		CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->sentStanzas.size());
		Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(0);
		CPPUNIT_ASSERT(stanzaContactOnRoster->getPayload<DeliveryReceipt>() != 0);
	}

private:
	boost::shared_ptr<Message> makeDeliveryReceiptTestMessage(const JID& from, const std::string& id) {
		boost::shared_ptr<Message> message = boost::make_shared<Message>();
		message->setFrom(from);
		message->setID(id);
		message->addPayload(boost::make_shared<DeliveryReceiptRequest>());
		return message;
	}

	size_t st(int i) {
		return static_cast<size_t>(i);
	}

private:
	JID jid_;
	ChatsManager* manager_;
	DummyStanzaChannel* stanzaChannel_;
	IQChannel* iqChannel_;
	IQRouter* iqRouter_;
	EventController* eventController_;
	ChatWindowFactory* chatWindowFactory_;
	JoinMUCWindowFactory* joinMUCWindowFactory_;
	NickResolver* nickResolver_;
	PresenceOracle* presenceOracle_;
	AvatarManager* avatarManager_;
	boost::shared_ptr<DiscoInfo> serverDiscoInfo_;
	XMPPRosterImpl* xmppRoster_;
	PresenceSender* presenceSender_;
	MockRepository* mocks_;
	UIEventStream* uiEventStream_;
	ChatListWindowFactory* chatListWindowFactory_;
	MUCSearchWindowFactory* mucSearchWindowFactory_;
	MUCRegistry* mucRegistry_;
	DirectedPresenceSender* directedPresenceSender_;
	EntityCapsManager* entityCapsManager_;
	CapsProvider* capsProvider_;
	MUCManager* mucManager_;
	DummySettingsProvider* settings_;
	ProfileSettingsProvider* profileSettings_;
	ChatListWindow* chatListWindow_;
	FileTransferOverview* ftOverview_;
	FileTransferManager* ftManager_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatsManagerTest);

