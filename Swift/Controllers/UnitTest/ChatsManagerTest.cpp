#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include "3rdParty/hippomocks.h"

#include "Swift/Controllers/ChatsManager.h"

#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Client/Client.h"
#include "Swift/Controllers/ChatController.h"
#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/MUCController.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Avatars/UnitTest/MockAvatarManager.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swift/Controllers/UnitTest/MockChatWindow.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Presence/PresenceOracle.h"


using namespace Swift;

class ChatsManagerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ChatsManagerTest);
	CPPUNIT_TEST(testFirstOpenWindowIncoming);
	CPPUNIT_TEST(testSecondOpenWindowIncoming);
	CPPUNIT_TEST(testFirstOpenWindowOutgoing);
	CPPUNIT_TEST(testFirstOpenWindowBareToFull);
	CPPUNIT_TEST(testSecondWindow);
	CPPUNIT_TEST(testUnbindRebind);
	CPPUNIT_TEST(testNoDuplicateUnbind);
	CPPUNIT_TEST_SUITE_END();
	
public:
	ChatsManagerTest() {};
	
	void setUp() {
		mocks_ = new MockRepository();
		jid_ = JID("test@test.com/resource");
		stanzaChannel_ = new DummyStanzaChannel();
		iqChannel_ = new DummyIQChannel();
		iqRouter_ = new IQRouter(iqChannel_);
		eventController_ = new EventController();
		chatWindowFactory_ = mocks_->InterfaceMock<ChatWindowFactory>();
		treeWidgetFactory_ = NULL;
		xmppRoster_ = boost::shared_ptr<XMPPRoster>(new XMPPRoster());
		nickResolver_ = new NickResolver(xmppRoster_);
		presenceOracle_ = new PresenceOracle(stanzaChannel_);
		serverDiscoInfo_ = boost::shared_ptr<DiscoInfo>(new DiscoInfo());
		presenceSender_ = NULL;
		manager_ = new ChatsManager(jid_, stanzaChannel_, iqRouter_, eventController_, chatWindowFactory_, treeWidgetFactory_, nickResolver_, presenceOracle_, serverDiscoInfo_, presenceSender_);
		avatarManager_ = new MockAvatarManager();
		manager_->setAvatarManager(avatarManager_);
	};
	
	void tearDown() {
		delete manager_;
		delete presenceSender_;
		delete avatarManager_;
		delete presenceOracle_;
		delete nickResolver_;
		delete treeWidgetFactory_;
		delete stanzaChannel_;
		delete iqChannel_;
		delete iqRouter_;
		delete mocks_;
	}

	void testFirstOpenWindowIncoming() {
		JID messageJID("testling@test.com/resource1");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID).Return(window);

		boost::shared_ptr<Message> message(new Message());
		message->setFrom(messageJID);
		String body("This is a legible message. >HEH@)oeueu");
		message->setBody(body);
		manager_->handleIncomingMessage(message);
		CPPUNIT_ASSERT_EQUAL(body, window->lastMessageBody_);
	}

	void testSecondOpenWindowIncoming() {
		JID messageJID1("testling@test.com/resource1");
		
		MockChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID1).Return(window1);

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(messageJID1);
		String body1("This is a legible message. >HEH@)oeueu");
		message1->setBody(body1);
		manager_->handleIncomingMessage(message1);
		CPPUNIT_ASSERT_EQUAL(body1, window1->lastMessageBody_);

		JID messageJID2("testling@test.com/resource2");
		
		MockChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID2).Return(window2);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(messageJID2);
		String body2("This is a legible message. .cmaulm.chul");
		message2->setBody(body2);
		manager_->handleIncomingMessage(message2);
		CPPUNIT_ASSERT_EQUAL(body2, window2->lastMessageBody_);
	}

	void testFirstOpenWindowOutgoing() {
		String messageJIDString("testling@test.com");
		
		ChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString)).Return(window);

		manager_->handleChatRequest(messageJIDString);
	}


	void testFirstOpenWindowBareToFull() {
		String bareJIDString("testling@test.com");
		String fullJIDString("testling@test.com/resource1");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString)).Return(window);
		manager_->handleChatRequest(bareJIDString);

		boost::shared_ptr<Message> message(new Message());
		message->setFrom(JID(fullJIDString));
		String body("This is a legible message. mjuga3089gm8G(*>M)@*(");
		message->setBody(body);
		manager_->handleIncomingMessage(message);
		CPPUNIT_ASSERT_EQUAL(body, window->lastMessageBody_);
	}

	void testSecondWindow() {
		String messageJIDString1("testling1@test.com");
		ChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString1)).Return(window1);
		manager_->handleChatRequest(messageJIDString1);

		String messageJIDString2("testling2@test.com");
		ChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString2)).Return(window2);

		manager_->handleChatRequest(messageJIDString2);
	}

	/** Complete cycle.
		Create unbound window.
		Bind it.
		Unbind it.
		Rebind it.
	 */
	void testUnbindRebind() {
		String bareJIDString("testling@test.com");
		String fullJIDString1("testling@test.com/resource1");
		String fullJIDString2("testling@test.com/resource2");
		
		MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString)).Return(window);
		manager_->handleChatRequest(bareJIDString);

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(JID(fullJIDString1));
		String messageBody1("This is a legible message.");
		message1->setBody(messageBody1);
		manager_->handleIncomingMessage(message1);
		CPPUNIT_ASSERT_EQUAL(messageBody1, window->lastMessageBody_);
		
		boost::shared_ptr<Presence> jid1Online(new Presence());
		jid1Online->setFrom(JID(fullJIDString1));
		boost::shared_ptr<Presence> jid1Offline(new Presence());
		jid1Offline->setFrom(JID(fullJIDString1));
		jid1Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid1Offline, jid1Online);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(JID(fullJIDString2));
		String messageBody2("This is another legible message.");
		message2->setBody(messageBody2);
		manager_->handleIncomingMessage(message2);
		CPPUNIT_ASSERT_EQUAL(messageBody2, window->lastMessageBody_);
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
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID1).Return(window1);

		boost::shared_ptr<Message> message1(new Message());
		message1->setFrom(messageJID1);
		message1->setBody("This is a legible message1.");
		manager_->handleIncomingMessage(message1);

		JID messageJID2("testling@test.com/resource2");
		
		MockChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID2).Return(window2);

		boost::shared_ptr<Message> message2(new Message());
		message2->setFrom(messageJID2);
		message2->setBody("This is a legible message2.");
		manager_->handleIncomingMessage(message2);

		boost::shared_ptr<Presence> jid1Online(new Presence());
		jid1Online->setFrom(JID(messageJID1));
		boost::shared_ptr<Presence> jid1Offline(new Presence());
		jid1Offline->setFrom(JID(messageJID1));
		jid1Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid1Offline, jid1Online);
 
		boost::shared_ptr<Presence> jid2Online(new Presence());
		jid2Online->setFrom(JID(messageJID2));
		boost::shared_ptr<Presence> jid2Offline(new Presence());
		jid2Offline->setFrom(JID(messageJID2));
		jid2Offline->setType(Presence::Unavailable);
		presenceOracle_->onPresenceChange(jid2Offline, jid2Online);

		JID messageJID3("testling@test.com/resource3");

		boost::shared_ptr<Message> message3(new Message());
		message3->setFrom(messageJID3);
		String body3("This is a legible message3.");
		message3->setBody(body3);
		manager_->handleIncomingMessage(message3);
		CPPUNIT_ASSERT_EQUAL(body3, window1->lastMessageBody_);

		boost::shared_ptr<Message> message2b(new Message());
		message2b->setFrom(messageJID2);
		String body2b("This is a legible message2b.");
		message2b->setBody(body2b);
		manager_->handleIncomingMessage(message2b);
		CPPUNIT_ASSERT_EQUAL(body2b, window2->lastMessageBody_);
	}
	
private:
	JID jid_;
	ChatsManager* manager_;
	StanzaChannel* stanzaChannel_;
	IQChannel* iqChannel_;
	IQRouter* iqRouter_;
	EventController* eventController_;
	ChatWindowFactory* chatWindowFactory_;
	TreeWidgetFactory* treeWidgetFactory_;
	NickResolver* nickResolver_;
	PresenceOracle* presenceOracle_;
	AvatarManager* avatarManager_;
	boost::shared_ptr<DiscoInfo> serverDiscoInfo_;
	boost::shared_ptr<XMPPRoster> xmppRoster_;
	PresenceSender* presenceSender_;
	MockRepository* mocks_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatsManagerTest);

