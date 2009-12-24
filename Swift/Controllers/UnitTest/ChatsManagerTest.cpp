#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include "3rdParty/hippomocks.h"

#include "Swift/Controllers/ChatsManager.h"

#include "Swift/Controllers/ChatWindow.h"
#include "Swift/Controllers/ChatWindowFactory.h"
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
	CPPUNIT_TEST(testFirstOpenWindowOutgoing);
	CPPUNIT_TEST(testFirstOpenWindowBareToFull);
	CPPUNIT_TEST(testSecondWindow);
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
		
		ChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
		mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID).Return(window);

		boost::shared_ptr<Message> message(new Message());
		message->setFrom(messageJID);
		message->setBody("This is a legible message.");
		manager_->handleIncomingMessage(message);
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
		message->setBody("This is a legible message.");
		manager_->handleIncomingMessage(message);
		/*FIXME: check the message got through. For now, checking that there isn't a new window created is useful enough.*/
		//CPPUNIT_ASSERT_EQUAL(fullJIDString, window->name_);
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

