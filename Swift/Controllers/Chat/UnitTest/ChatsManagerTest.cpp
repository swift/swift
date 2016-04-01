/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <map>
#include <set>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <hippomocks.h>

#include <Swiften/Avatars/AvatarMemoryStorage.h>
#include <Swiften/Avatars/NullAvatarManager.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Disco/DummyEntityCapsProvider.h>
#include <Swiften/Elements/DeliveryReceipt.h>
#include <Swiften/Elements/DeliveryReceiptRequest.h>
#include <Swiften/FileTransfer/UnitTest/DummyFileTransferManager.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/MUC/MUCManager.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Presence/StanzaChannelPresenceSender.h>
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>
#include <Swiften/Whiteboard/WhiteboardSessionManager.h>

#include <Swift/Controllers/Chat/ChatController.h>
#include <Swift/Controllers/Chat/ChatsManager.h>
#include <Swift/Controllers/Chat/MUCController.h>
#include <Swift/Controllers/Chat/UnitTest/MockChatListWindow.h>
#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>
#include <Swift/Controllers/ProfileSettingsProvider.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/DummySettingsProvider.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ChatListWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/WhiteboardWindowFactory.h>
#include <Swift/Controllers/UnitTest/MockChatWindow.h>
#include <Swift/Controllers/WhiteboardManager.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

using namespace Swift;

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
    CPPUNIT_TEST(testChatControllerFullJIDBindingOnMessageAndNotReceipt);
    CPPUNIT_TEST(testChatControllerFullJIDBindingOnTypingAndNotActive);
    CPPUNIT_TEST(testChatControllerPMPresenceHandling);
    CPPUNIT_TEST(testLocalMUCServiceDiscoveryResetOnDisconnect);
    CPPUNIT_TEST(testChatControllerHighlightingNotificationTesting);
    CPPUNIT_TEST(testChatControllerHighlightingNotificationDeduplicateSounds);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        mocks_ = new MockRepository();
        jid_ = JID("test@test.com/resource");
        stanzaChannel_ = new DummyStanzaChannel();
        iqChannel_ = new DummyIQChannel();
        iqRouter_ = new IQRouter(iqChannel_);
//        capsProvider_ = new DummyCapsProvider();
        eventController_ = new EventController();
        chatWindowFactory_ = mocks_->InterfaceMock<ChatWindowFactory>();
        joinMUCWindowFactory_ = mocks_->InterfaceMock<JoinMUCWindowFactory>();
        xmppRoster_ = new XMPPRosterImpl();
        mucRegistry_ = new MUCRegistry();
        nickResolver_ = new NickResolver(jid_.toBare(), xmppRoster_, nullptr, mucRegistry_);
        presenceOracle_ = new PresenceOracle(stanzaChannel_, xmppRoster_);
        serverDiscoInfo_ = std::make_shared<DiscoInfo>();
        presenceSender_ = new StanzaChannelPresenceSender(stanzaChannel_);
        directedPresenceSender_ = new DirectedPresenceSender(presenceSender_);
        mucManager_ = new MUCManager(stanzaChannel_, iqRouter_, directedPresenceSender_, mucRegistry_);
        uiEventStream_ = new UIEventStream();
//        entityCapsManager_ = new EntityCapsManager(capsProvider_, stanzaChannel_);
        entityCapsProvider_ = new DummyEntityCapsProvider();
        chatListWindowFactory_ = mocks_->InterfaceMock<ChatListWindowFactory>();
        mucSearchWindowFactory_ = mocks_->InterfaceMock<MUCSearchWindowFactory>();
        settings_ = new DummySettingsProvider();
        profileSettings_ = new ProfileSettingsProvider("a", settings_);
        chatListWindow_ = new MockChatListWindow();
        ftManager_ = new DummyFileTransferManager();
        ftOverview_ = new FileTransferOverview(ftManager_);
        avatarManager_ = new NullAvatarManager();
        wbSessionManager_ = new WhiteboardSessionManager(iqRouter_, stanzaChannel_, presenceOracle_, entityCapsProvider_);
        wbManager_ = new WhiteboardManager(whiteboardWindowFactory_, uiEventStream_, nickResolver_, wbSessionManager_);
        highlightManager_ = new HighlightManager(settings_);
        handledHighlightActions_ = 0;
        soundsPlayed_.clear();
        highlightManager_->onHighlight.connect(boost::bind(&ChatsManagerTest::handleHighlightAction, this, _1));

        crypto_ = PlatformCryptoProvider::create();
        vcardStorage_ = new VCardMemoryStorage(crypto_);
        vcardManager_ = new VCardManager(jid_, iqRouter_, vcardStorage_);
        mocks_->ExpectCall(chatListWindowFactory_, ChatListWindowFactory::createChatListWindow).With(uiEventStream_).Return(chatListWindow_);
        clientBlockListManager_ = new ClientBlockListManager(iqRouter_);
        manager_ = new ChatsManager(jid_, stanzaChannel_, iqRouter_, eventController_, chatWindowFactory_, joinMUCWindowFactory_, nickResolver_, presenceOracle_, directedPresenceSender_, uiEventStream_, chatListWindowFactory_, true, nullptr, mucRegistry_, entityCapsProvider_, mucManager_, mucSearchWindowFactory_, profileSettings_, ftOverview_, xmppRoster_, false, settings_, nullptr, wbManager_, highlightManager_, clientBlockListManager_, emoticons_, vcardManager_);

        manager_->setAvatarManager(avatarManager_);
    }

    void tearDown() {
        delete highlightManager_;
        //delete chatListWindowFactory
        delete profileSettings_;
        delete avatarManager_;
        delete manager_;
        delete clientBlockListManager_;
        delete vcardManager_;
        delete vcardStorage_;
        delete crypto_;
        delete ftOverview_;
        delete ftManager_;
        delete wbSessionManager_;
        delete wbManager_;
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
        delete entityCapsProvider_;
        delete chatListWindow_;
        delete mocks_;
        delete settings_;
    }

    void testFirstOpenWindowIncoming() {
        JID messageJID("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
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

        std::shared_ptr<Message> message1(new Message());
        message1->setFrom(messageJID1);
        std::string body1("This is a legible message. >HEH@)oeueu");
        message1->setBody(body1);
        manager_->handleIncomingMessage(message1);
        CPPUNIT_ASSERT_EQUAL(body1, window1->lastMessageBody_);

        JID messageJID2("testling@test.com/resource2");

        //MockChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        //mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID2, uiEventStream_).Return(window2);

        std::shared_ptr<Message> message2(new Message());
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

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString)));
    }


    void testFirstOpenWindowBareToFull() {
        std::string bareJIDString("testling@test.com");
        std::string fullJIDString("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString), uiEventStream_).Return(window);
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(bareJIDString)));

        std::shared_ptr<Message> message(new Message());
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
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString1)));

        std::string messageJIDString2("testling2@test.com");
        ChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString2), uiEventStream_).Return(window2);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString2)));
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
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(bareJIDString)));

        std::shared_ptr<Message> message1(new Message());
        message1->setFrom(JID(fullJIDString1));
        std::string messageBody1("This is a legible message.");
        message1->setBody(messageBody1);
        manager_->handleIncomingMessage(message1);
        CPPUNIT_ASSERT_EQUAL(messageBody1, window->lastMessageBody_);

        std::shared_ptr<Presence> jid1Online(new Presence());
        jid1Online->setFrom(JID(fullJIDString1));
        std::shared_ptr<Presence> jid1Offline(new Presence());
        jid1Offline->setFrom(JID(fullJIDString1));
        jid1Offline->setType(Presence::Unavailable);
        presenceOracle_->onPresenceChange(jid1Offline);

        std::shared_ptr<Message> message2(new Message());
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
        uiEventStream_->send(std::make_shared<JoinMUCUIEvent>(muc, std::string("nick")));


        std::string messageJIDString1("testling@test.com/1");
        ChatWindow* window1 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString1), uiEventStream_).Return(window1);
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString1)));

        std::string messageJIDString2("testling@test.com/2");
        ChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString2), uiEventStream_).Return(window2);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString2)));

        std::string messageJIDString3("testling@test.com/3");
        ChatWindow* window3 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString3), uiEventStream_).Return(window3);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString3)));

        /* Refetch an earlier window */
        /* We do not expect a new window to be created */
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString1)));

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

        std::shared_ptr<Message> message1(new Message());
        message1->setFrom(messageJID1);
        message1->setBody("This is a legible message1.");
        manager_->handleIncomingMessage(message1);

        JID messageJID2("testling@test.com/resource2");

        //MockChatWindow* window2 = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        //mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID2, uiEventStream_).Return(window2);

        std::shared_ptr<Message> message2(new Message());
        message2->setFrom(messageJID2);
        message2->setBody("This is a legible message2.");
        manager_->handleIncomingMessage(message2);

        std::shared_ptr<Presence> jid1Online(new Presence());
        jid1Online->setFrom(JID(messageJID1));
        std::shared_ptr<Presence> jid1Offline(new Presence());
        jid1Offline->setFrom(JID(messageJID1));
        jid1Offline->setType(Presence::Unavailable);
        presenceOracle_->onPresenceChange(jid1Offline);

        std::shared_ptr<Presence> jid2Online(new Presence());
        jid2Online->setFrom(JID(messageJID2));
        std::shared_ptr<Presence> jid2Offline(new Presence());
        jid2Offline->setFrom(JID(messageJID2));
        jid2Offline->setType(Presence::Unavailable);
        presenceOracle_->onPresenceChange(jid2Offline);

        JID messageJID3("testling@test.com/resource3");

        std::shared_ptr<Message> message3(new Message());
        message3->setFrom(messageJID3);
        std::string body3("This is a legible message3.");
        message3->setBody(body3);
        manager_->handleIncomingMessage(message3);
        CPPUNIT_ASSERT_EQUAL(body3, window1->lastMessageBody_);

        std::shared_ptr<Message> message2b(new Message());
        message2b->setFrom(messageJID2);
        std::string body2b("This is a legible message2b.");
        message2b->setBody(body2b);
        manager_->handleIncomingMessage(message2b);
        CPPUNIT_ASSERT_EQUAL(body2b, window1->lastMessageBody_);
    }

    /**
     *    Test that ChatController doesn't send receipts anymore after removal of the contact from the roster.
     */
    void testChatControllerPresenceAccessUpdatedOnRemoveFromRoster() {
        JID messageJID("testling@test.com/resource1");
        xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), RosterItemPayload::Both);

        MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);
        settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

        std::shared_ptr<Message> message = makeDeliveryReceiptTestMessage(messageJID, "1");
        manager_->handleIncomingMessage(message);
        Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(0);
        CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->sentStanzas.size());
        CPPUNIT_ASSERT(stanzaContactOnRoster->getPayload<DeliveryReceipt>() != nullptr);

        xmppRoster_->removeContact(messageJID);

        message->setID("2");
        manager_->handleIncomingMessage(message);
        CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->sentStanzas.size());
    }

    /**
     *    Test that ChatController sends receipts after the contact has been added to the roster.
     */
    void testChatControllerPresenceAccessUpdatedOnAddToRoster() {
        JID messageJID("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);
        settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

        std::shared_ptr<Message> message = makeDeliveryReceiptTestMessage(messageJID, "1");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(st(0), stanzaChannel_->sentStanzas.size());

        xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), RosterItemPayload::Both);
        message->setID("2");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->sentStanzas.size());
        Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(0);
        CPPUNIT_ASSERT(stanzaContactOnRoster->getPayload<DeliveryReceipt>() != nullptr);
    }

    /**
     *    Test that ChatController sends receipts if requested after change from subscription state To to subscription state Both.
     */
    void testChatControllerPresenceAccessUpdatedOnSubscriptionChangeToBoth() {
        testhelperChatControllerPresenceAccessUpdatedOnSubscriptionChangeReceiptsAllowed(RosterItemPayload::To, RosterItemPayload::Both);
    }

    /**
     *    Test that ChatController sends receipts if requested after change from subscription state To to subscription state From.
     */
    void testChatControllerPresenceAccessUpdatedOnSubscriptionChangeToFrom() {
        testhelperChatControllerPresenceAccessUpdatedOnSubscriptionChangeReceiptsAllowed(RosterItemPayload::To, RosterItemPayload::From);
    }

    void testChatControllerFullJIDBindingOnMessageAndNotReceipt() {
        JID ownJID("test@test.com/resource");
        JID sender("foo@test.com");
        std::vector<JID> senderResource;
        senderResource.push_back(sender.withResource("resourceA"));
        senderResource.push_back(sender.withResource("resourceB"));

        // We support delivery receipts.
        settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

        // Open chat window to a sender.
        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(sender, uiEventStream_).Return(window);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(sender));

        foreach(const JID& senderJID, senderResource) {
            // The sender supports delivery receipts.
            DiscoInfo::ref disco = std::make_shared<DiscoInfo>();
            disco->addFeature(DiscoInfo::MessageDeliveryReceiptsFeature);
            entityCapsProvider_->caps[senderJID] = disco;

            // The sender is online.
            Presence::ref senderPresence = std::make_shared<Presence>();
            senderPresence->setFrom(senderJID);
            senderPresence->setTo(ownJID);
            stanzaChannel_->onPresenceReceived(senderPresence);

            entityCapsProvider_->onCapsChanged(senderJID);
        }

        // Send first message.
        window->onSendMessageRequest("hello there", false);

        // A bare message is send because no resources is bound.
        CPPUNIT_ASSERT_EQUAL(sender, stanzaChannel_->getStanzaAtIndex<Message>(0)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(0)->getPayload<DeliveryReceiptRequest>());

        // Two resources respond with message receipts.
        foreach(const JID& senderJID, senderResource) {
            Message::ref receiptReply = std::make_shared<Message>();
            receiptReply->setFrom(senderJID);
            receiptReply->setTo(ownJID);

            std::shared_ptr<DeliveryReceipt> receipt = std::make_shared<DeliveryReceipt>();
            receipt->setReceivedID(stanzaChannel_->getStanzaAtIndex<Message>(0)->getID());
            receiptReply->addPayload(receipt);
            manager_->handleIncomingMessage(receiptReply);
        }

        // Send second message.
        window->onSendMessageRequest("how are you?", false);

        // A bare message is send because no resources is bound.
        CPPUNIT_ASSERT_EQUAL(sender, stanzaChannel_->getStanzaAtIndex<Message>(1)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(1)->getPayload<DeliveryReceiptRequest>());

        // Two resources respond with message receipts.
        foreach(const JID& senderJID, senderResource) {
            Message::ref receiptReply = std::make_shared<Message>();
            receiptReply->setFrom(senderJID);
            receiptReply->setTo(ownJID);

            std::shared_ptr<DeliveryReceipt> receipt = std::make_shared<DeliveryReceipt>();
            receipt->setReceivedID(stanzaChannel_->getStanzaAtIndex<Message>(1)->getID());
            receiptReply->addPayload(receipt);
            manager_->handleIncomingMessage(receiptReply);
        }

        // Reply with a message including a body text.
        Message::ref reply = std::make_shared<Message>();
        reply->setFrom(senderResource[0]);
        reply->setTo(ownJID);
        reply->setBody("fine.");
        manager_->handleIncomingMessage(reply);

        // Send third message.
        window->onSendMessageRequest("great to hear.", false);

        // The chat session is bound to the full JID of the first resource.
        CPPUNIT_ASSERT_EQUAL(senderResource[0], stanzaChannel_->getStanzaAtIndex<Message>(2)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(2)->getPayload<DeliveryReceiptRequest>());

        // Receive random receipt from second sender resource.
        reply = std::make_shared<Message>();
        reply->setFrom(senderResource[1]);
        reply->setTo(ownJID);

        std::shared_ptr<DeliveryReceipt> receipt = std::make_shared<DeliveryReceipt>();
        receipt->setReceivedID(stanzaChannel_->getStanzaAtIndex<Message>(2)->getID());
        reply->addPayload(receipt);
        manager_->handleIncomingMessage(reply);

        // Send forth message.
        window->onSendMessageRequest("what else is new?", false);

        // The chat session is bound to the full JID of the first resource.
        CPPUNIT_ASSERT_EQUAL(senderResource[0], stanzaChannel_->getStanzaAtIndex<Message>(3)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(3)->getPayload<DeliveryReceiptRequest>());

        // Reply with a message including a body text from second resource.
        reply = std::make_shared<Message>();
        reply->setFrom(senderResource[1]);
        reply->setTo(ownJID);
        reply->setBody("nothing.");
        manager_->handleIncomingMessage(reply);

        // Send fifth message.
        window->onSendMessageRequest("okay", false);

        // The chat session is now bound to the full JID of the second resource.
        CPPUNIT_ASSERT_EQUAL(senderResource[1], stanzaChannel_->getStanzaAtIndex<Message>(4)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(4)->getPayload<DeliveryReceiptRequest>());
    }

    void testChatControllerFullJIDBindingOnTypingAndNotActive() {
        JID ownJID("test@test.com/resource");
        JID sender("foo@test.com");
        std::vector<JID> senderResource;
        senderResource.push_back(sender.withResource("resourceA"));
        senderResource.push_back(sender.withResource("resourceB"));

        // We support delivery receipts.
        settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

        // Open chat window to a sender.
        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(sender, uiEventStream_).Return(window);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(sender));

        foreach(const JID& senderJID, senderResource) {
            // The sender supports delivery receipts.
            DiscoInfo::ref disco = std::make_shared<DiscoInfo>();
            disco->addFeature(DiscoInfo::MessageDeliveryReceiptsFeature);
            entityCapsProvider_->caps[senderJID] = disco;

            // The sender is online.
            Presence::ref senderPresence = std::make_shared<Presence>();
            senderPresence->setFrom(senderJID);
            senderPresence->setTo(ownJID);
            stanzaChannel_->onPresenceReceived(senderPresence);

            entityCapsProvider_->onCapsChanged(senderJID);
        }

        // Send first message.
        window->onSendMessageRequest("hello there", false);

        // A bare message is send because no resources is bound.
        CPPUNIT_ASSERT_EQUAL(sender, stanzaChannel_->getStanzaAtIndex<Message>(0)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(0)->getPayload<DeliveryReceiptRequest>());

        // Two resources respond with message receipts.
        foreach(const JID& senderJID, senderResource) {
            Message::ref reply = std::make_shared<Message>();
            reply->setFrom(senderJID);
            reply->setTo(ownJID);

            std::shared_ptr<ChatState> csn = std::make_shared<ChatState>();
            csn->setChatState(ChatState::Active);
            reply->addPayload(csn);
            manager_->handleIncomingMessage(reply);
        }

        // Send second message.
        window->onSendMessageRequest("how are you?", false);

        // A bare message is send because no resources is bound.
        CPPUNIT_ASSERT_EQUAL(sender, stanzaChannel_->getStanzaAtIndex<Message>(1)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(1)->getPayload<DeliveryReceiptRequest>());

        // Two resources respond with message receipts.
        foreach(const JID& senderJID, senderResource) {
            Message::ref receiptReply = std::make_shared<Message>();
            receiptReply->setFrom(senderJID);
            receiptReply->setTo(ownJID);

            std::shared_ptr<DeliveryReceipt> receipt = std::make_shared<DeliveryReceipt>();
            receipt->setReceivedID(stanzaChannel_->getStanzaAtIndex<Message>(1)->getID());
            receiptReply->addPayload(receipt);
            manager_->handleIncomingMessage(receiptReply);
        }

        // Reply with a message including a CSN.
        Message::ref reply = std::make_shared<Message>();
        reply->setFrom(senderResource[0]);
        reply->setTo(ownJID);

        std::shared_ptr<ChatState> csn = std::make_shared<ChatState>();
        csn->setChatState(ChatState::Composing);
        reply->addPayload(csn);
        manager_->handleIncomingMessage(reply);

        // Send third message.
        window->onSendMessageRequest("great to hear.", false);

        // The chat session is now bound to the full JID of the first resource due to its recent composing message.
        CPPUNIT_ASSERT_EQUAL(senderResource[0], stanzaChannel_->getStanzaAtIndex<Message>(2)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(2)->getPayload<DeliveryReceiptRequest>());

        // Reply with a message including a CSN from the other resource.
        reply = std::make_shared<Message>();
        reply->setFrom(senderResource[1]);
        reply->setTo(ownJID);

        csn = std::make_shared<ChatState>();
        csn->setChatState(ChatState::Composing);
        reply->addPayload(csn);
        manager_->handleIncomingMessage(reply);

        // Send third message.
        window->onSendMessageRequest("ping.", false);

        // The chat session is now bound to the full JID of the second resource due to its recent composing message.
        CPPUNIT_ASSERT_EQUAL(senderResource[1], stanzaChannel_->getStanzaAtIndex<Message>(3)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(3)->getPayload<DeliveryReceiptRequest>());
    }

    void testChatControllerPMPresenceHandling() {
        JID participantA = JID("test@rooms.test.com/participantA");
        JID participantB = JID("test@rooms.test.com/participantB");

        mucRegistry_->addMUC("test@rooms.test.com");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(participantA, uiEventStream_).Return(window);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(participantA)));

        Presence::ref presence = Presence::create();
        presence->setFrom(participantA);
        presence->setShow(StatusShow::Online);
        stanzaChannel_->onPresenceReceived(presence);
        CPPUNIT_ASSERT_EQUAL(std::string("participantA has become available."), MockChatWindow::bodyFromMessage(window->lastAddedPresence_));

        presence = Presence::create();
        presence->setFrom(participantB);
        presence->setShow(StatusShow::Away);
        stanzaChannel_->onPresenceReceived(presence);

        presence = Presence::create();
        presence->setFrom(participantA);
        presence->setShow(StatusShow::None);
        presence->setType(Presence::Unavailable);
        stanzaChannel_->onPresenceReceived(presence);
        CPPUNIT_ASSERT_EQUAL(std::string("participantA has gone offline."), MockChatWindow::bodyFromMessage(window->lastReplacedMessage_));
    }

    void testLocalMUCServiceDiscoveryResetOnDisconnect() {
        JID ownJID("test@test.com/resource");
        JID sender("foo@test.com");

        manager_->setOnline(true);

        // Open chat window to a sender.
        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(sender, uiEventStream_).Return(window);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(sender));

        CPPUNIT_ASSERT_EQUAL(false, window->impromptuMUCSupported_);

        std::shared_ptr<IQ> infoRequest= iqChannel_->iqs_[1];
        std::shared_ptr<IQ> infoResponse = IQ::createResult(infoRequest->getFrom(), infoRequest->getTo(), infoRequest->getID());

        DiscoInfo info;
        info.addIdentity(DiscoInfo::Identity("Shakespearean Chat Service", "conference", "text"));
        info.addFeature("http://jabber.org/protocol/muc");
        infoResponse->addPayload(std::make_shared<DiscoInfo>(info));
        iqChannel_->onIQReceived(infoResponse);

        CPPUNIT_ASSERT_EQUAL(true, window->impromptuMUCSupported_);
        manager_->setOnline(false);
        CPPUNIT_ASSERT_EQUAL(false, window->impromptuMUCSupported_);
    }

    void testhelperChatControllerPresenceAccessUpdatedOnSubscriptionChangeReceiptsAllowed(RosterItemPayload::Subscription from, RosterItemPayload::Subscription to) {
        JID messageJID("testling@test.com/resource1");
        xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), from);

        MockChatWindow* window = new MockChatWindow();//mocks_->InterfaceMock<ChatWindow>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);
        settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

        std::shared_ptr<Message> message = makeDeliveryReceiptTestMessage(messageJID, "1");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(st(0), stanzaChannel_->sentStanzas.size());

        xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), to);
        message->setID("2");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->sentStanzas.size());
        Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(0);
        CPPUNIT_ASSERT(stanzaContactOnRoster->getPayload<DeliveryReceipt>() != nullptr);
    }

    void testChatControllerHighlightingNotificationTesting() {
        HighlightRule keywordRuleA;
        keywordRuleA.setMatchChat(true);
        std::vector<std::string> keywordsA;
        keywordsA.push_back("Romeo");
        keywordRuleA.setKeywords(keywordsA);
        keywordRuleA.getAction().setTextColor("yellow");
        keywordRuleA.getAction().setPlaySound(true);
        highlightManager_->insertRule(0, keywordRuleA);

        HighlightRule keywordRuleB;
        keywordRuleB.setMatchChat(true);
        std::vector<std::string> keywordsB;
        keywordsB.push_back("Juliet");
        keywordRuleB.setKeywords(keywordsB);
        keywordRuleB.getAction().setTextColor("green");
        keywordRuleB.getAction().setPlaySound(true);
        keywordRuleB.getAction().setSoundFile("/tmp/someotherfile.wav");
        highlightManager_->insertRule(0, keywordRuleB);

        JID messageJID = JID("testling@test.com");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("This message should cause two sounds: Juliet and Romeo.");
        message->setBody(body);
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(2, handledHighlightActions_);
        CPPUNIT_ASSERT(soundsPlayed_.find(keywordRuleA.getAction().getSoundFile()) != soundsPlayed_.end());
        CPPUNIT_ASSERT(soundsPlayed_.find(keywordRuleB.getAction().getSoundFile()) != soundsPlayed_.end());
    }

    void testChatControllerHighlightingNotificationDeduplicateSounds() {
        HighlightRule keywordRuleA;
        keywordRuleA.setMatchChat(true);
        std::vector<std::string> keywordsA;
        keywordsA.push_back("Romeo");
        keywordRuleA.setKeywords(keywordsA);
        keywordRuleA.getAction().setTextColor("yellow");
        keywordRuleA.getAction().setPlaySound(true);
        highlightManager_->insertRule(0, keywordRuleA);

        HighlightRule keywordRuleB;
        keywordRuleB.setMatchChat(true);
        std::vector<std::string> keywordsB;
        keywordsB.push_back("Juliet");
        keywordRuleB.setKeywords(keywordsB);
        keywordRuleB.getAction().setTextColor("green");
        keywordRuleB.getAction().setPlaySound(true);
        highlightManager_->insertRule(0, keywordRuleB);

        JID messageJID = JID("testling@test.com");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("This message should cause one sound, because both actions have the same sound: Juliet and Romeo.");
        message->setBody(body);
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(1, handledHighlightActions_);
        CPPUNIT_ASSERT(soundsPlayed_.find(keywordRuleA.getAction().getSoundFile()) != soundsPlayed_.end());
        CPPUNIT_ASSERT(soundsPlayed_.find(keywordRuleB.getAction().getSoundFile()) != soundsPlayed_.end());
    }

private:
    std::shared_ptr<Message> makeDeliveryReceiptTestMessage(const JID& from, const std::string& id) {
        std::shared_ptr<Message> message = std::make_shared<Message>();
        message->setFrom(from);
        message->setID(id);
        message->setBody("This will cause the window to open");
        message->addPayload(std::make_shared<DeliveryReceiptRequest>());
        return message;
    }

    size_t st(int i) {
        return static_cast<size_t>(i);
    }

    void handleHighlightAction(const HighlightAction& action) {
        handledHighlightActions_++;
        if (action.playSound()) {
            soundsPlayed_.insert(action.getSoundFile());
        }
    }

private:
    JID jid_;
    ChatsManager* manager_;
    DummyStanzaChannel* stanzaChannel_;
    DummyIQChannel* iqChannel_;
    IQRouter* iqRouter_;
    EventController* eventController_;
    ChatWindowFactory* chatWindowFactory_;
    JoinMUCWindowFactory* joinMUCWindowFactory_;
    NickResolver* nickResolver_;
    PresenceOracle* presenceOracle_;
    AvatarManager* avatarManager_;
    std::shared_ptr<DiscoInfo> serverDiscoInfo_;
    XMPPRosterImpl* xmppRoster_;
    PresenceSender* presenceSender_;
    MockRepository* mocks_;
    UIEventStream* uiEventStream_;
    ChatListWindowFactory* chatListWindowFactory_;
    WhiteboardWindowFactory* whiteboardWindowFactory_;
    MUCSearchWindowFactory* mucSearchWindowFactory_;
    MUCRegistry* mucRegistry_;
    DirectedPresenceSender* directedPresenceSender_;
    DummyEntityCapsProvider* entityCapsProvider_;
    MUCManager* mucManager_;
    DummySettingsProvider* settings_;
    ProfileSettingsProvider* profileSettings_;
    ChatListWindow* chatListWindow_;
    FileTransferOverview* ftOverview_;
    FileTransferManager* ftManager_;
    WhiteboardSessionManager* wbSessionManager_;
    WhiteboardManager* wbManager_;
    HighlightManager* highlightManager_;
    ClientBlockListManager* clientBlockListManager_;
    VCardManager* vcardManager_;
    CryptoProvider* crypto_;
    VCardStorage* vcardStorage_;
    std::map<std::string, std::string> emoticons_;
    int handledHighlightActions_;
    std::set<std::string> soundsPlayed_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatsManagerTest);

