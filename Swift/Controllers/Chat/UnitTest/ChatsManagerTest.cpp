/*
 * Copyright (c) 2010-2017 Isode Limited.
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
#include <Swiften/Base/Log.h>
#include <Swiften/Base/LogSerializers.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Disco/DummyEntityCapsProvider.h>
#include <Swiften/Elements/CarbonsReceived.h>
#include <Swiften/Elements/CarbonsSent.h>
#include <Swiften/Elements/DeliveryReceipt.h>
#include <Swiften/Elements/DeliveryReceiptRequest.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Elements/MUCInvitationPayload.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/FileTransfer/UnitTest/DummyFileTransferManager.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/MUC/MUCManager.h>
#include <Swiften/Network/DummyTimerFactory.h>
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
#include <Swift/Controllers/EventNotifier.h>
#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>
#include <Swift/Controllers/ProfileSettingsProvider.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/DummySettingsProvider.h>
#include <Swift/Controllers/UIEvents/CreateImpromptuMUCUIEvent.h>
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

#include <SwifTools/Notifier/Notifier.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swiften/MUC/UnitTest/MockMUC.h>

using namespace Swift;

class DummyNotifier : public Notifier {
    public:
        virtual void showMessage(
            Type type,
            const std::string& subject,
            const std::string& description,
            const boost::filesystem::path& picture,
            boost::function<void()> callback) {
            notifications.push_back({type, subject, description, picture, callback});
        }

        /** Remove any pending callbacks. */
        virtual void purgeCallbacks() {

        }

    public:
        struct Notification {
            Type type;
            std::string subject;
            std::string description;
            boost::filesystem::path picture;
            boost::function<void()> callback;
        };

    public:
        std::vector<Notification> notifications;
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
    CPPUNIT_TEST(testChatControllerFullJIDBindingOnMessageAndNotReceipt);
    CPPUNIT_TEST(testChatControllerFullJIDBindingOnTypingAndNotActive);
    CPPUNIT_TEST(testLocalMUCServiceDiscoveryResetOnDisconnect);
    CPPUNIT_TEST(testPresenceChangeDoesNotReplaceMUCInvite);
    CPPUNIT_TEST(testNotSplittingMUCPresenceJoinLeaveLinesOnChatStateNotifications);

    // MUC PM Tests
    CPPUNIT_TEST(testChatControllerPMPresenceHandling);
    CPPUNIT_TEST(testChatControllerMucPmUnavailableErrorHandling);

    // Highlighting tests
    CPPUNIT_TEST(testChatControllerHighlightingNotificationTesting);
    CPPUNIT_TEST(testChatControllerHighlightingNotificationDeduplicateSounds);
    CPPUNIT_TEST(testChatControllerHighlightingNotificationKeyword);

    CPPUNIT_TEST(testChatControllerMeMessageHandling);
    CPPUNIT_TEST(testRestartingMUCComponentCrash);
    CPPUNIT_TEST(testChatControllerMeMessageHandlingInMUC);

    // Carbons tests
    CPPUNIT_TEST(testCarbonsForwardedIncomingMessageToSecondResource);
    CPPUNIT_TEST(testCarbonsForwardedOutgoingMessageFromSecondResource);
    CPPUNIT_TEST(testCarbonsForwardedIncomingDuplicates);

    // Message correction tests
    CPPUNIT_TEST(testChatControllerMessageCorrectionCorrectReplaceID);
    CPPUNIT_TEST(testChatControllerMessageCorrectionIncorrectReplaceID);
    CPPUNIT_TEST(testChatControllerMessageCorrectionReplaceBySameResource);
    CPPUNIT_TEST(testChatControllerMessageCorrectionReplaceByOtherResource);
    CPPUNIT_TEST(testMUCControllerMessageCorrectionNoIDMatchRequired);

    // Chat window title tests
    CPPUNIT_TEST(testImpromptuChatTitle);
    CPPUNIT_TEST(testImpromptuChatWindowTitle);
    CPPUNIT_TEST(testStandardMUCChatWindowTitle);

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        mocks_ = new MockRepository();
        notifier_ = std::unique_ptr<DummyNotifier>(new DummyNotifier());
        jid_ = JID("test@test.com/resource");
        stanzaChannel_ = new DummyStanzaChannel();
        iqRouter_ = new IQRouter(stanzaChannel_);
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
        entityCapsProvider_ = new DummyEntityCapsProvider();
        chatListWindowFactory_ = mocks_->InterfaceMock<ChatListWindowFactory>();
        mucSearchWindowFactory_ = mocks_->InterfaceMock<MUCSearchWindowFactory>();
        settings_ = new DummySettingsProvider();
        profileSettings_ = new ProfileSettingsProvider("a", settings_);
        chatListWindow_ = new MockChatListWindow();
        ftManager_ = new DummyFileTransferManager();
        ftOverview_ = new FileTransferOverview(ftManager_);
        avatarManager_ = new NullAvatarManager();
        eventNotifier_ = new EventNotifier(eventController_, notifier_.get(), avatarManager_, nickResolver_);
        wbSessionManager_ = new WhiteboardSessionManager(iqRouter_, stanzaChannel_, presenceOracle_, entityCapsProvider_);
        wbManager_ = new WhiteboardManager(whiteboardWindowFactory_, uiEventStream_, nickResolver_, wbSessionManager_);
        highlightManager_ = new HighlightManager(settings_);
        highlightManager_->resetToDefaultConfiguration();
        handledHighlightActions_ = 0;
        soundsPlayed_.clear();
        highlightManager_->onHighlight.connect(boost::bind(&ChatsManagerTest::handleHighlightAction, this, _1));

        crypto_ = PlatformCryptoProvider::create();
        vcardStorage_ = new VCardMemoryStorage(crypto_);
        vcardManager_ = new VCardManager(jid_, iqRouter_, vcardStorage_);
        mocks_->ExpectCall(chatListWindowFactory_, ChatListWindowFactory::createChatListWindow).With(uiEventStream_).Return(chatListWindow_);
        clientBlockListManager_ = new ClientBlockListManager(iqRouter_);
        timerFactory_ = new DummyTimerFactory();
        manager_ = new ChatsManager(jid_, stanzaChannel_, iqRouter_, eventController_, chatWindowFactory_, joinMUCWindowFactory_, nickResolver_, presenceOracle_, directedPresenceSender_, uiEventStream_, chatListWindowFactory_, true, timerFactory_, mucRegistry_, entityCapsProvider_, mucManager_, mucSearchWindowFactory_, profileSettings_, ftOverview_, xmppRoster_, false, settings_, nullptr, wbManager_, highlightManager_, clientBlockListManager_, emoticons_, vcardManager_);

        manager_->setAvatarManager(avatarManager_);
    }

    void tearDown() {
        delete highlightManager_;
        delete profileSettings_;
        delete eventNotifier_;
        delete avatarManager_;
        delete manager_;
        delete timerFactory_;
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
        delete iqRouter_;
        delete stanzaChannel_;
        delete eventController_;
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
        CPPUNIT_ASSERT_EQUAL(body, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
    }

    void testSecondOpenWindowIncoming() {
        JID messageJID1("testling@test.com/resource1");

        MockChatWindow* window1 = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID1, uiEventStream_).Return(window1);

        std::shared_ptr<Message> message1(new Message());
        message1->setFrom(messageJID1);
        std::string body1("This is a legible message. >HEH@)oeueu");
        message1->setBody(body1);
        manager_->handleIncomingMessage(message1);
        CPPUNIT_ASSERT_EQUAL(body1, MockChatWindow::bodyFromMessage(window1->lastAddedMessage_));

        JID messageJID2("testling@test.com/resource2");

        std::shared_ptr<Message> message2(new Message());
        message2->setFrom(messageJID2);
        std::string body2("This is a legible message. .cmaulm.chul");
        message2->setBody(body2);
        manager_->handleIncomingMessage(message2);
        CPPUNIT_ASSERT_EQUAL(body2, MockChatWindow::bodyFromMessage(window1->lastAddedMessage_));
    }

    void testFirstOpenWindowOutgoing() {
        std::string messageJIDString("testling@test.com");

        ChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString), uiEventStream_).Return(window);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString)));
    }


    void testFirstOpenWindowBareToFull() {
        std::string bareJIDString("testling@test.com");
        std::string fullJIDString("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString), uiEventStream_).Return(window);
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(bareJIDString)));

        std::shared_ptr<Message> message(new Message());
        message->setFrom(JID(fullJIDString));
        std::string body("This is a legible message. mjuga3089gm8G(*>M)@*(");
        message->setBody(body);
        manager_->handleIncomingMessage(message);
        CPPUNIT_ASSERT_EQUAL(body, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
    }

    void testSecondWindow() {
        std::string messageJIDString1("testling1@test.com");
        ChatWindow* window1 = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString1), uiEventStream_).Return(window1);
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString1)));

        std::string messageJIDString2("testling2@test.com");
        ChatWindow* window2 = new MockChatWindow();
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

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(bareJIDString), uiEventStream_).Return(window);
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(bareJIDString)));

        std::shared_ptr<Message> message1(new Message());
        message1->setFrom(JID(fullJIDString1));
        std::string messageBody1("This is a legible message.");
        message1->setBody(messageBody1);
        manager_->handleIncomingMessage(message1);
        CPPUNIT_ASSERT_EQUAL(messageBody1, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

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
        CPPUNIT_ASSERT_EQUAL(messageBody2, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
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
        ChatWindow* window1 = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString1), uiEventStream_).Return(window1);
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString1)));

        std::string messageJIDString2("testling@test.com/2");
        ChatWindow* window2 = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(JID(messageJIDString2), uiEventStream_).Return(window2);

        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(JID(messageJIDString2)));

        std::string messageJIDString3("testling@test.com/3");
        ChatWindow* window3 = new MockChatWindow();
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

        MockChatWindow* window1 = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID1, uiEventStream_).Return(window1);

        std::shared_ptr<Message> message1(new Message());
        message1->setFrom(messageJID1);
        message1->setBody("This is a legible message1.");
        manager_->handleIncomingMessage(message1);

        JID messageJID2("testling@test.com/resource2");

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
        CPPUNIT_ASSERT_EQUAL(body3, MockChatWindow::bodyFromMessage(window1->lastAddedMessage_));

        std::shared_ptr<Message> message2b(new Message());
        message2b->setFrom(messageJID2);
        std::string body2b("This is a legible message2b.");
        message2b->setBody(body2b);
        manager_->handleIncomingMessage(message2b);
        CPPUNIT_ASSERT_EQUAL(body2b, MockChatWindow::bodyFromMessage(window1->lastAddedMessage_));
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
        Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(1);
        CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->countSentStanzaOfType<Message>());
        CPPUNIT_ASSERT(stanzaContactOnRoster->getPayload<DeliveryReceipt>() != nullptr);

        xmppRoster_->removeContact(messageJID);

        message->setID("2");
        manager_->handleIncomingMessage(message);
        CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->countSentStanzaOfType<Message>());
    }

    /**
     *    Test that ChatController sends receipts after the contact has been added to the roster.
     */
    void testChatControllerPresenceAccessUpdatedOnAddToRoster() {
        JID messageJID("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);
        settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, true);

        std::shared_ptr<Message> message = makeDeliveryReceiptTestMessage(messageJID, "1");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(st(0), stanzaChannel_->countSentStanzaOfType<Message>());

        xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), RosterItemPayload::Both);
        message->setID("2");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->countSentStanzaOfType<Message>());
        Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(1);
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

        for (const auto& senderJID : senderResource) {
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
        CPPUNIT_ASSERT_EQUAL(sender, stanzaChannel_->getStanzaAtIndex<Message>(1)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(1)->getPayload<DeliveryReceiptRequest>());

        // Two resources respond with message receipts.
        for (const auto& senderJID : senderResource) {
            Message::ref receiptReply = std::make_shared<Message>();
            receiptReply->setFrom(senderJID);
            receiptReply->setTo(ownJID);

            std::shared_ptr<DeliveryReceipt> receipt = std::make_shared<DeliveryReceipt>();
            receipt->setReceivedID(stanzaChannel_->getStanzaAtIndex<Message>(1)->getID());
            receiptReply->addPayload(receipt);
            manager_->handleIncomingMessage(receiptReply);
        }

        // Send second message.
        window->onSendMessageRequest("how are you?", false);

        // A bare message is send because no resources is bound.
        CPPUNIT_ASSERT_EQUAL(sender, stanzaChannel_->getStanzaAtIndex<Message>(1)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(1)->getPayload<DeliveryReceiptRequest>());

        // Two resources respond with message receipts.
        for (const auto& senderJID : senderResource) {
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
        CPPUNIT_ASSERT_EQUAL(senderResource[0], stanzaChannel_->getStanzaAtIndex<Message>(3)->getTo());
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
        CPPUNIT_ASSERT_EQUAL(senderResource[1], stanzaChannel_->getStanzaAtIndex<Message>(5)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(5)->getPayload<DeliveryReceiptRequest>());
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

        for (const auto& senderJID : senderResource) {
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
        CPPUNIT_ASSERT_EQUAL(sender, stanzaChannel_->getStanzaAtIndex<Message>(1)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(1)->getPayload<DeliveryReceiptRequest>());

        // Two resources respond with message receipts.
        for (const auto& senderJID : senderResource) {
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
        for (const auto& senderJID : senderResource) {
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
        CPPUNIT_ASSERT_EQUAL(senderResource[0], stanzaChannel_->getStanzaAtIndex<Message>(3)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(3)->getPayload<DeliveryReceiptRequest>());

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
        CPPUNIT_ASSERT_EQUAL(senderResource[1], stanzaChannel_->getStanzaAtIndex<Message>(4)->getTo());
        CPPUNIT_ASSERT(stanzaChannel_->getStanzaAtIndex<Message>(4)->getPayload<DeliveryReceiptRequest>());
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
        CPPUNIT_ASSERT_EQUAL(std::string("participantA has gone offline."), MockChatWindow::bodyFromMessage(window->lastReplacedLastMessage_));
    }

    void testChatControllerMucPmUnavailableErrorHandling() {
        auto mucJID = JID("test@rooms.test.com");
        auto participantA = mucJID.withResource("participantA");
        auto participantB = mucJID.withResource("participantB");

        auto mucWindow = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(mucJID, uiEventStream_).Return(mucWindow);
        uiEventStream_->send(std::make_shared<JoinMUCUIEvent>(mucJID, participantB.getResource()));
        CPPUNIT_ASSERT_EQUAL(true, mucWindow->mucType_.is_initialized());

        auto window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(participantA, uiEventStream_).Return(window);
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(participantA));
        CPPUNIT_ASSERT_EQUAL(false, window->mucType_.is_initialized());

        Presence::ref presence = Presence::create();
        presence->setFrom(participantA);
        presence->setShow(StatusShow::Online);
        stanzaChannel_->onPresenceReceived(presence);
        CPPUNIT_ASSERT_EQUAL(std::string("participantA has become available."), MockChatWindow::bodyFromMessage(window->lastAddedPresence_));

        // send message to participantA
        auto messageBody = std::string("message body to send");
        window->onSendMessageRequest(messageBody, false);
        auto sendMessageStanza = stanzaChannel_->getStanzaAtIndex<Message>(2);
        CPPUNIT_ASSERT_EQUAL(messageBody, *sendMessageStanza->getBody());

        // receive reply with error
        auto messageErrorReply = std::make_shared<Message>();
        messageErrorReply->setID(stanzaChannel_->getNewIQID());
        messageErrorReply->setType(Message::Error);
        messageErrorReply->setFrom(participantA);
        messageErrorReply->setTo(jid_);
        messageErrorReply->addPayload(std::make_shared<ErrorPayload>(ErrorPayload::ItemNotFound, ErrorPayload::Cancel, "Recipient not in room"));

        auto lastMUCWindowErrorMessageBeforeError = MockChatWindow::bodyFromMessage(mucWindow->lastAddedErrorMessage_);
        manager_->handleIncomingMessage(messageErrorReply);

        // assert that error is not routed to MUC window
        CPPUNIT_ASSERT_EQUAL(lastMUCWindowErrorMessageBeforeError,  MockChatWindow::bodyFromMessage(mucWindow->lastAddedErrorMessage_));
        // assert that error is routed to PM
        CPPUNIT_ASSERT_EQUAL(std::string("This user could not be found in the room."), MockChatWindow::bodyFromMessage(window->lastAddedErrorMessage_));
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

        std::shared_ptr<IQ> infoRequest = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[1]);
        std::shared_ptr<IQ> infoResponse = IQ::createResult(infoRequest->getFrom(), infoRequest->getTo(), infoRequest->getID());

        DiscoInfo info;
        info.addIdentity(DiscoInfo::Identity("Shakespearean Chat Service", "conference", "text"));
        info.addFeature("http://jabber.org/protocol/muc");
        infoResponse->addPayload(std::make_shared<DiscoInfo>(info));
        stanzaChannel_->onIQReceived(infoResponse);

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

        CPPUNIT_ASSERT_EQUAL(st(0), stanzaChannel_->countSentStanzaOfType<Message>());

        xmppRoster_->addContact(messageJID, "foo", std::vector<std::string>(), to);
        message->setID("2");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(st(1), stanzaChannel_->countSentStanzaOfType<Message>());
        Stanza::ref stanzaContactOnRoster = stanzaChannel_->getStanzaAtIndex<Stanza>(1);
        CPPUNIT_ASSERT(stanzaContactOnRoster->getPayload<DeliveryReceipt>() != nullptr);
    }

    void testChatControllerHighlightingNotificationTesting() {
        HighlightConfiguration::KeywordHightlight keywordRuleA;
        keywordRuleA.keyword = "Romeo";
        keywordRuleA.action.setFrontColor(boost::optional<std::string>("yellow"));
        keywordRuleA.action.setSoundFilePath(boost::optional<std::string>(""));
        highlightManager_->getConfiguration()->keywordHighlights.push_back(keywordRuleA);

        HighlightConfiguration::KeywordHightlight keywordRuleB;
        keywordRuleB.keyword = "Juliet";
        keywordRuleB.action.setFrontColor(boost::optional<std::string>("green"));
        keywordRuleB.action.setSoundFilePath(boost::optional<std::string>("/tmp/someotherfile.wav"));
        highlightManager_->getConfiguration()->keywordHighlights.push_back(keywordRuleB);

        JID messageJID = JID("testling@test.com");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("This message should cause two sounds: Juliet and Romeo.");
        message->setBody(body);
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(2, handledHighlightActions_);
        CPPUNIT_ASSERT(soundsPlayed_.find(keywordRuleA.action.getSoundFilePath().get_value_or("")) != soundsPlayed_.end());
        CPPUNIT_ASSERT(soundsPlayed_.find(keywordRuleB.action.getSoundFilePath().get_value_or("")) != soundsPlayed_.end());
        CPPUNIT_ASSERT_EQUAL(size_t(1), notifier_->notifications.size());
    }

    void testChatControllerHighlightingNotificationDeduplicateSounds() {
        auto keywordRuleA = HighlightConfiguration::KeywordHightlight();
        keywordRuleA.keyword = "Romeo";
        keywordRuleA.action.setFrontColor(boost::optional<std::string>("yellow"));
        keywordRuleA.action.setSoundFilePath(boost::optional<std::string>(""));
        highlightManager_->getConfiguration()->keywordHighlights.push_back(keywordRuleA);

        auto keywordRuleB = HighlightConfiguration::KeywordHightlight();
        keywordRuleB.keyword = "Juliet";
        keywordRuleB.action.setFrontColor(boost::optional<std::string>("green"));
        keywordRuleB.action.setSoundFilePath(boost::optional<std::string>(""));
        highlightManager_->getConfiguration()->keywordHighlights.push_back(keywordRuleB);

        JID messageJID = JID("testling@test.com");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("This message should cause one sound, because both actions have the same sound: Juliet and Romeo.");
        message->setBody(body);
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(1, handledHighlightActions_);
        CPPUNIT_ASSERT(soundsPlayed_.find(keywordRuleA.action.getSoundFilePath().get_value_or("")) != soundsPlayed_.end());
        CPPUNIT_ASSERT(soundsPlayed_.find(keywordRuleB.action.getSoundFilePath().get_value_or("")) != soundsPlayed_.end());
        CPPUNIT_ASSERT_EQUAL(size_t(1), notifier_->notifications.size());
    }

    void testChatControllerHighlightingNotificationKeyword() {
        auto keywordRuleA = HighlightConfiguration::KeywordHightlight();
        keywordRuleA.keyword = "Swift";
        keywordRuleA.action.setFrontColor(boost::optional<std::string>("yellow"));
        keywordRuleA.action.setSoundFilePath(boost::optional<std::string>(""));
        keywordRuleA.action.setSystemNotificationEnabled(true);
        highlightManager_->getConfiguration()->keywordHighlights.push_back(keywordRuleA);

        JID messageJID = JID("testling@test.com");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("Let's see if the Swift highlight kicks off a system notification.");
        message->setBody(body);
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(size_t(2), notifier_->notifications.size());
    }

    void testChatControllerMeMessageHandling() {
        JID messageJID("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("/me is feeling delighted.");
        message->setBody(body);
        manager_->handleIncomingMessage(message);
        CPPUNIT_ASSERT_EQUAL(std::string("is feeling delighted."), window->bodyFromMessage(window->lastAddedAction_));
    }

   void testRestartingMUCComponentCrash() {
        JID mucJID = JID("teaparty@rooms.wonderland.lit");
        JID self = JID("girl@wonderland.lit/rabbithole");
        std::string nick = "aLiCe";

        MockChatWindow* window;

        auto genRemoteMUCPresence = [=]() {
            auto presence = Presence::create();
            presence->setFrom(mucJID.withResource(nick));
            presence->setTo(self);
            return presence;
        };

        // User rejoins.
        window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(mucJID, uiEventStream_).Return(window);

        // Join room
        {
            auto joinRoomEvent = std::make_shared<JoinMUCUIEvent>(mucJID, boost::optional<std::string>(), nick);
            uiEventStream_->send(joinRoomEvent);
        }

        {
            auto firstPresence = genRemoteMUCPresence();
            firstPresence->setType(Presence::Unavailable);
            auto userPayload = std::make_shared<MUCUserPayload>();
            userPayload->addItem(MUCItem(MUCOccupant::Owner, MUCOccupant::NoRole));
            firstPresence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(firstPresence);
        }
        CPPUNIT_ASSERT_EQUAL(std::string("Couldn't enter room: Unable to enter this room."), MockChatWindow::bodyFromMessage(window->lastAddedErrorMessage_));

        {
            auto presence = genRemoteMUCPresence();
            presence->setType(Presence::Unavailable);
            auto userPayload = std::make_shared<MUCUserPayload>();
            userPayload->addStatusCode(303);
            auto item = MUCItem(MUCOccupant::Owner, self, MUCOccupant::Moderator);
            item.nick = nick;
            userPayload->addItem(item);
            userPayload->addStatusCode(110);
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }
    }

    void testChatControllerMeMessageHandlingInMUC() {
        JID mucJID("mucroom@rooms.test.com");
        std::string nickname = "toodles";

        //highlightManager_->resetToDefaultConfiguration();

        // add highlight rule for 'foo'
        HighlightConfiguration::KeywordHightlight keywordHighlight;
        keywordHighlight.keyword = "foo";
        keywordHighlight.action.setBackColor(boost::optional<std::string>("green"));
        highlightManager_->getConfiguration()->keywordHighlights.push_back(keywordHighlight);

        HighlightConfiguration::KeywordHightlight keywordHighlightNotification;
        keywordHighlightNotification.keyword = "Swift";
        keywordHighlightNotification.action.setBackColor(boost::optional<std::string>("green"));
        keywordHighlightNotification.action.setSystemNotificationEnabled(true);
        highlightManager_->getConfiguration()->keywordHighlights.push_back(keywordHighlightNotification);

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(mucJID, uiEventStream_).Return(window);

        uiEventStream_->send(std::make_shared<JoinMUCUIEvent>(mucJID, boost::optional<std::string>(), nickname));

        auto genRemoteMUCPresence = [=]() {
            auto presence = Presence::create();
            presence->setFrom(mucJID.withResource(nickname));
            presence->setTo(jid_);
            return presence;
        };

        {
            auto presence = genRemoteMUCPresence();
            auto userPayload = std::make_shared<MUCUserPayload>();
            userPayload->addStatusCode(110);
            userPayload->addItem(MUCItem(MUCOccupant::Owner, jid_, MUCOccupant::Moderator));
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }

        {
            auto presence = genRemoteMUCPresence();
            presence->setFrom(mucJID.withResource("someDifferentNickname"));
            auto userPayload = std::make_shared<MUCUserPayload>();
            userPayload->addItem(MUCItem(MUCOccupant::Member, JID("foo@bar.com"), MUCOccupant::Moderator));
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }

        window->onSendMessageRequest("/me sends a test message with foo", false);

        window->resetLastMessages();
        {
            Message::ref mucMirrored = std::make_shared<Message>();
            mucMirrored->setFrom(mucJID.withResource(nickname));
            mucMirrored->setTo(jid_);
            mucMirrored->setType(Message::Groupchat);
            mucMirrored->setBody("/me sends a test message with foo");
            manager_->handleIncomingMessage(mucMirrored);
        }
        CPPUNIT_ASSERT_EQUAL(std::string("sends a test message with foo"), window->bodyFromMessage(window->lastAddedAction_));

        window->resetLastMessages();
        {
            Message::ref mucMirrored = std::make_shared<Message>();
            mucMirrored->setFrom(mucJID.withResource("someDifferentNickname"));
            mucMirrored->setTo(jid_);
            mucMirrored->setType(Message::Groupchat);
            mucMirrored->setBody("/me says hello with a test message with foo and foo");
            manager_->handleIncomingMessage(mucMirrored);
        }
        CPPUNIT_ASSERT_EQUAL(std::string("says hello with a test message with foo and foo"), window->bodyFromMessage(window->lastAddedAction_));

        window->resetLastMessages();
        {
            Message::ref keywordMessage = std::make_shared<Message>();
            keywordMessage->setFrom(mucJID.withResource("someOtherDifferentNickname"));
            keywordMessage->setTo(jid_);
            keywordMessage->setType(Message::Groupchat);
            keywordMessage->setBody("Let's mention Swift and see what happens.");
            manager_->handleIncomingMessage(keywordMessage);
        }
        CPPUNIT_ASSERT_EQUAL(std::string("Let's mention Swift and see what happens."), window->bodyFromMessage(window->lastAddedMessage_));
        CPPUNIT_ASSERT_EQUAL(size_t(1), notifier_->notifications.size());
    }

    void testPresenceChangeDoesNotReplaceMUCInvite() {
        JID messageJID("testling@test.com/resource1");

        auto generateIncomingPresence = [=](Presence::Type type) {
            auto presence = std::make_shared<Presence>();
            presence->setType(type);
            presence->setFrom(messageJID);
            presence->setTo(jid_);
            return presence;
        };

        stanzaChannel_->onPresenceReceived(generateIncomingPresence(Presence::Available));

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("This is a legible message. >HEH@)oeueu");
        message->setBody(body);
        manager_->handleIncomingMessage(message);
        CPPUNIT_ASSERT_EQUAL(body, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

        auto incomingMUCInvite = std::make_shared<Message>();
        incomingMUCInvite->setFrom(messageJID);

        auto invitePayload = std::make_shared<MUCInvitationPayload>();
        invitePayload->setJID("room@muc.service.com");
        incomingMUCInvite->addPayload(invitePayload);

        stanzaChannel_->onPresenceReceived(generateIncomingPresence(Presence::Unavailable));
        stanzaChannel_->onPresenceReceived(generateIncomingPresence(Presence::Available));

        window->resetLastMessages();

        manager_->handleIncomingMessage(incomingMUCInvite);
        CPPUNIT_ASSERT_EQUAL(JID("room@muc.service.com"), window->lastMUCInvitationJID_);

        stanzaChannel_->onPresenceReceived(generateIncomingPresence(Presence::Unavailable));
        CPPUNIT_ASSERT_EQUAL(std::string(""), MockChatWindow::bodyFromMessage(window->lastReplacedLastMessage_));
        CPPUNIT_ASSERT_EQUAL(std::string("testling@test.com has gone offline."), MockChatWindow::bodyFromMessage(window->lastAddedPresence_));
    }

    void testNotSplittingMUCPresenceJoinLeaveLinesOnChatStateNotifications() {
        JID mucJID("mucroom@rooms.test.com");
        std::string nickname = "toodles";

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(mucJID, uiEventStream_).Return(window);

        uiEventStream_->send(std::make_shared<JoinMUCUIEvent>(mucJID, boost::optional<std::string>(), nickname));

        auto genRemoteMUCPresence = [=]() {
            auto presence = Presence::create();
            presence->setFrom(mucJID.withResource(nickname));
            presence->setTo(jid_);
            return presence;
        };

        {
            auto presence = genRemoteMUCPresence();
            auto userPayload = std::make_shared<MUCUserPayload>();
            userPayload->addStatusCode(110);
            userPayload->addItem(MUCItem(MUCOccupant::Owner, jid_, MUCOccupant::Moderator));
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }

        {
            auto presence = genRemoteMUCPresence();
            presence->setFrom(mucJID.withResource("someDifferentNickname"));
            auto userPayload = std::make_shared<MUCUserPayload>();
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }
        CPPUNIT_ASSERT_EQUAL(std::string("someDifferentNickname has entered the room."), window->bodyFromMessage(window->lastAddedPresence_));
        CPPUNIT_ASSERT_EQUAL(std::string(), window->bodyFromMessage(window->lastReplacedLastMessage_));
        window->resetLastMessages();

        {
            auto presence = genRemoteMUCPresence();
            presence->setFrom(mucJID.withResource("Romeo"));
            auto userPayload = std::make_shared<MUCUserPayload>();
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }
        CPPUNIT_ASSERT_EQUAL(std::string(), window->bodyFromMessage(window->lastAddedPresence_));
        CPPUNIT_ASSERT_EQUAL(std::string("someDifferentNickname and Romeo have entered the room"), window->bodyFromMessage(window->lastReplacedLastMessage_));
        window->resetLastMessages();

        {
            auto message = std::make_shared<Message>();
            message->setFrom(mucJID.withResource("Romeo"));
            message->setTo(mucJID);
            message->setType(Message::Groupchat);
            message->addPayload(std::make_shared<ChatState>(ChatState::Composing));
            manager_->handleIncomingMessage(message);
        }

        {
            auto presence = genRemoteMUCPresence();
            presence->setFrom(mucJID.withResource("Juliet"));
            auto userPayload = std::make_shared<MUCUserPayload>();
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }
        CPPUNIT_ASSERT_EQUAL(std::string(), window->bodyFromMessage(window->lastAddedPresence_));
        CPPUNIT_ASSERT_EQUAL(std::string("someDifferentNickname, Romeo and Juliet have entered the room"), window->bodyFromMessage(window->lastReplacedLastMessage_));
    }

    template <typename CarbonsType>
    Message::ref createCarbonsMessage(std::shared_ptr<CarbonsType> carbons, std::shared_ptr<Message> forwardedMessage) {
        auto messageWrapper = std::make_shared<Message>();
        messageWrapper->setFrom(jid_.toBare());
        messageWrapper->setTo(jid_);
        messageWrapper->setType(Message::Chat);

        messageWrapper->addPayload(carbons);
        auto forwarded = std::make_shared<Forwarded>();
        carbons->setForwarded(forwarded);
        forwarded->setStanza(forwardedMessage);
        return messageWrapper;
    }

    void testCarbonsForwardedIncomingMessageToSecondResource() {
        JID messageJID("testling@test.com/resource1");
        JID jid2 = jid_.toBare().withResource("someOtherResource");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("This is a legible message. >HEH@)oeueu");
        message->setBody(body);
        manager_->handleIncomingMessage(message);
        CPPUNIT_ASSERT_EQUAL(body, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

        // incoming carbons message from another resource
        {
            auto originalMessage = std::make_shared<Message>();
            originalMessage->setFrom(messageJID);
            originalMessage->setTo(jid2);
            originalMessage->setType(Message::Chat);
            std::string forwardedBody = "Some further text.";
            originalMessage->setBody(forwardedBody);

            auto messageWrapper = createCarbonsMessage(std::make_shared<CarbonsReceived>(), originalMessage);

            manager_->handleIncomingMessage(messageWrapper);

            CPPUNIT_ASSERT_EQUAL(forwardedBody, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
            CPPUNIT_ASSERT_EQUAL(false, window->lastAddedMessageSenderIsSelf_);
        }
    }

    void testCarbonsForwardedOutgoingMessageFromSecondResource() {
        JID messageJID("testling@test.com/resource1");
        JID jid2 = jid_.toBare().withResource("someOtherResource");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("This is a legible message. >HEH@)oeueu");
        message->setBody(body);
        manager_->handleIncomingMessage(message);
        CPPUNIT_ASSERT_EQUAL(body, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

        // incoming carbons message from another resource
        {
            auto originalMessage = std::make_shared<Message>();
            originalMessage->setFrom(jid2);
            originalMessage->setTo(messageJID);
            originalMessage->setType(Message::Chat);
            originalMessage->setID("abcdefg123456");
            std::string forwardedBody = "Some text my other resource sent.";
            originalMessage->setBody(forwardedBody);
            originalMessage->addPayload(std::make_shared<DeliveryReceiptRequest>());

            auto messageWrapper = createCarbonsMessage(std::make_shared<CarbonsSent>(), originalMessage);

            manager_->handleIncomingMessage(messageWrapper);

            CPPUNIT_ASSERT_EQUAL(forwardedBody, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
            CPPUNIT_ASSERT_EQUAL(true, window->lastAddedMessageSenderIsSelf_);
            CPPUNIT_ASSERT_EQUAL(size_t(1), window->receiptChanges_.size());
            CPPUNIT_ASSERT_EQUAL(ChatWindow::ReceiptRequested, window->receiptChanges_[0].second);
        }

        // incoming carbons message for the received delivery receipt to the other resource
        {
            auto originalMessage = std::make_shared<Message>();
            originalMessage->setFrom(messageJID);
            originalMessage->setTo(jid2);
            originalMessage->setType(Message::Chat);
            originalMessage->addPayload(std::make_shared<DeliveryReceipt>("abcdefg123456"));

            auto messageWrapper = createCarbonsMessage(std::make_shared<CarbonsReceived>(), originalMessage);

            manager_->handleIncomingMessage(messageWrapper);

            CPPUNIT_ASSERT_EQUAL(size_t(2), window->receiptChanges_.size());
            CPPUNIT_ASSERT_EQUAL(ChatWindow::ReceiptReceived, window->receiptChanges_[1].second);
        }
    }

    void testCarbonsForwardedIncomingDuplicates() {
        JID messageJID("testling@test.com/resource1");
        JID jid2 = jid_.toBare().withResource("someOtherResource");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        std::shared_ptr<Message> message(new Message());
        message->setFrom(messageJID);
        std::string body("This is a legible message. >HEH@)oeueu");
        message->setBody(body);
        manager_->handleIncomingMessage(message);
        CPPUNIT_ASSERT_EQUAL(body, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

        // incoming carbons message from another resource and duplicate of it
        {
            auto originalMessage = std::make_shared<Message>();
            originalMessage->setFrom(messageJID);
            originalMessage->setTo(jid2);
            originalMessage->setID("BDD82F0B-2523-48BF-B8CA-17B23A314BC2");
            originalMessage->setType(Message::Chat);
            std::string forwardedBody = "Some further text.";
            originalMessage->setBody(forwardedBody);

            auto messageWrapper = createCarbonsMessage(std::make_shared<CarbonsReceived>(), originalMessage);

            manager_->handleIncomingMessage(messageWrapper);

            CPPUNIT_ASSERT_EQUAL(forwardedBody, MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
            CPPUNIT_ASSERT_EQUAL(false, window->lastAddedMessageSenderIsSelf_);
            window->resetLastMessages();

            messageWrapper = createCarbonsMessage(std::make_shared<CarbonsReceived>(), originalMessage);
            manager_->handleIncomingMessage(messageWrapper);
            CPPUNIT_ASSERT_EQUAL(std::string(), MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
            CPPUNIT_ASSERT_EQUAL(false, window->lastAddedMessageSenderIsSelf_);
        }
    }

    /**
     * This test case ensures correct handling of the ideal case where the replace
     * message refers to a message with a known ID. This results in the last
     * message being replaced.
     */
    void testChatControllerMessageCorrectionCorrectReplaceID() {
        JID messageJID("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        auto message = std::make_shared<Message>();
        message->setFrom(messageJID);
        message->setTo(jid_);
        message->setType(Message::Chat);
        message->setBody("text before edit");
        message->setID("someID");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(std::string("text before edit"), MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

        message = std::make_shared<Message>();
        message->setFrom(messageJID);
        message->setTo(jid_);
        message->setType(Message::Chat);
        message->setBody("text after edit");
        message->addPayload(std::make_shared<Replace>("someID"));
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(std::string("text before edit"), MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
        CPPUNIT_ASSERT_EQUAL(std::string("text after edit"), MockChatWindow::bodyFromMessage(window->lastReplacedMessage_));
    }

    /**
     * This test case ensures correct handling of the case where the replace
     * message refers to a message with a unknown ID. The replace message should
     * be treated like a non-repalce message in this case, with no replacement
     * occuring.
     */
    void testChatControllerMessageCorrectionIncorrectReplaceID() {
        JID messageJID("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        auto message = std::make_shared<Message>();
        message->setFrom(messageJID);
        message->setTo(jid_);
        message->setType(Message::Chat);
        message->setBody("text before edit");
        message->setID("someID");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(std::string("text before edit"), MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

        message = std::make_shared<Message>();
        message->setFrom(messageJID);
        message->setTo(jid_);
        message->setType(Message::Chat);
        message->setBody("text after failed edit");
        message->addPayload(std::make_shared<Replace>("wrongID"));
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(std::string("text after failed edit"), MockChatWindow::bodyFromMessage(window->lastAddedMessage_));
        CPPUNIT_ASSERT_EQUAL(std::string(""), MockChatWindow::bodyFromMessage(window->lastReplacedMessage_));
    }

    void testChatControllerMessageCorrectionReplaceBySameResource() {
        JID messageJID("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        auto message = std::make_shared<Message>();
        message->setFrom(messageJID);
        message->setTo(jid_);
        message->setType(Message::Chat);
        message->setBody("text before edit");
        message->setID("someID");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(std::string("text before edit"), MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

        message = std::make_shared<Message>();
        message->setFrom(messageJID);
        message->setTo(jid_);
        message->setType(Message::Chat);
        message->setBody("text after edit");
        message->addPayload(std::make_shared<Replace>("someID"));
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(std::string("text after edit"), MockChatWindow::bodyFromMessage(window->lastReplacedMessage_));
    }

    void testChatControllerMessageCorrectionReplaceByOtherResource() {
        JID messageJID("testling@test.com/resource1");

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(messageJID, uiEventStream_).Return(window);

        auto message = std::make_shared<Message>();
        message->setFrom(messageJID);
        message->setTo(jid_);
        message->setType(Message::Chat);
        message->setBody("text before edit");
        message->setID("someID");
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(std::string("text before edit"), MockChatWindow::bodyFromMessage(window->lastAddedMessage_));

        message = std::make_shared<Message>();
        message->setFrom(messageJID.toBare().withResource("resource2"));
        message->setTo(jid_);
        message->setType(Message::Chat);
        message->setBody("text after edit");
        message->addPayload(std::make_shared<Replace>("someID"));
        manager_->handleIncomingMessage(message);

        CPPUNIT_ASSERT_EQUAL(std::string("text after edit"), MockChatWindow::bodyFromMessage(window->lastReplacedMessage_));
    }

    void testMUCControllerMessageCorrectionNoIDMatchRequired() {
        JID mucJID("SomeMUCRoom@test.com");
        manager_->setOnline(true);

        // Open chat window to a sender.
        MockChatWindow* window = new MockChatWindow();

        std::vector<JID> jids;
        jids.emplace_back("foo@test.com");
        jids.emplace_back("bar@test.com");

        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(mucJID, uiEventStream_).Return(window);

        auto nickname = std::string("SomeNickName");
        // Join room
        {
            auto joinRoomEvent = std::make_shared<JoinMUCUIEvent>(mucJID, boost::optional<std::string>(), nickname);
            uiEventStream_->send(joinRoomEvent);
        }

        auto genRemoteMUCPresence = [=]() {
            auto presence = Presence::create();
            presence->setFrom(mucJID.withResource(nickname));
            presence->setTo(jid_);
            return presence;
        };

        {
            auto presence = genRemoteMUCPresence();
            auto userPayload = std::make_shared<MUCUserPayload>();
            userPayload->addStatusCode(110);
            userPayload->addItem(MUCItem(MUCOccupant::Owner, jid_, MUCOccupant::Moderator));
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }

        {
            auto presence = genRemoteMUCPresence();
            presence->setFrom(mucJID.withResource("someDifferentNickname"));
            auto userPayload = std::make_shared<MUCUserPayload>();
            userPayload->addItem(MUCItem(MUCOccupant::Member, JID("foo@bar.com"), MUCOccupant::Moderator));
            presence->addPayload(userPayload);
            stanzaChannel_->onPresenceReceived(presence);
        }

        {
            Message::ref mucMirrored = std::make_shared<Message>();
            mucMirrored->setFrom(mucJID.withResource(nickname));
            mucMirrored->setTo(jid_);
            mucMirrored->setType(Message::Groupchat);
            mucMirrored->setID("fooBlaID_1");
            mucMirrored->setBody("Some misssssspelled message.");
            manager_->handleIncomingMessage(mucMirrored);
        }
        CPPUNIT_ASSERT_EQUAL(std::string("Some misssssspelled message."), window->bodyFromMessage(window->lastAddedMessage_));

        // Replace message with non-matching ID
        {
            Message::ref mucMirrored = std::make_shared<Message>();
            mucMirrored->setFrom(mucJID.withResource(nickname));
            mucMirrored->setTo(jid_);
            mucMirrored->setType(Message::Groupchat);
            mucMirrored->setID("fooBlaID_3");
            mucMirrored->setBody("Some correctly spelled message.");
            mucMirrored->addPayload(std::make_shared<Replace>("fooBlaID_2"));
            manager_->handleIncomingMessage(mucMirrored);
        }
        CPPUNIT_ASSERT_EQUAL(std::string("Some correctly spelled message."), window->bodyFromMessage(window->lastReplacedMessage_));
    }

    void impromptuChatSetup(MockChatWindow* window) {
        stanzaChannel_->uniqueIDs_ = true;
        JID mucJID("795B7BBE-9099-4A0D-81BA-C816F78E275C@test.com");
        manager_->setOnline(true);

        std::shared_ptr<IQ> infoRequest = std::dynamic_pointer_cast<IQ>(stanzaChannel_->sentStanzas[1]);
        CPPUNIT_ASSERT(infoRequest);

        std::shared_ptr<IQ> infoResponse = IQ::createResult(infoRequest->getFrom(), infoRequest->getTo(), infoRequest->getID());

        DiscoInfo info;
        info.addIdentity(DiscoInfo::Identity("Shakespearean Chat Service", "conference", "text"));
        info.addFeature("http://jabber.org/protocol/muc");
        infoResponse->addPayload(std::make_shared<DiscoInfo>(info));
        stanzaChannel_->onIQReceived(infoResponse);

        std::vector<JID> jids;
        jids.emplace_back("foo@test.com");
        jids.emplace_back("bar@test.com");

        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(mucJID, uiEventStream_).Return(window);
        uiEventStream_->send(std::make_shared<CreateImpromptuMUCUIEvent>(jids, mucJID, ""));
        CPPUNIT_ASSERT_EQUAL(std::string("bar@test.com, foo@test.com"), manager_->getRecentChats()[0].getTitle());

        auto mucJoinPresence = std::dynamic_pointer_cast<Presence>(stanzaChannel_->sentStanzas[2]);
        CPPUNIT_ASSERT(mucJoinPresence);

        // MUC presence reply
        auto mucResponse = Presence::create();
        mucResponse->setTo(jid_);
        mucResponse->setFrom(mucJoinPresence->getTo());
        mucResponse->addPayload([]() {
            auto mucUser = std::make_shared<MUCUserPayload>();
            mucUser->addItem(MUCItem(MUCOccupant::Member, MUCOccupant::Participant));
            mucUser->addStatusCode(MUCUserPayload::StatusCode(110));
            mucUser->addStatusCode(MUCUserPayload::StatusCode(210));
            return mucUser;
        }());
        stanzaChannel_->onPresenceReceived(mucResponse);

        // Before people join the impromptu room, the title is based on names coming from Roster
        CPPUNIT_ASSERT_EQUAL(std::string("bar@test.com, foo@test.com"), manager_->getRecentChats()[0].getTitle());

        auto mucParticipantJoined = [&](const JID& jid) {
            auto participantJoinedPresence = Presence::create();
            participantJoinedPresence->setTo(jid_);
            participantJoinedPresence->setFrom(mucJID.withResource(jid.toString()));
            auto mucUser = std::make_shared<MUCUserPayload>();
            mucUser->addItem(MUCItem(MUCOccupant::Member, MUCOccupant::Participant));
            participantJoinedPresence->addPayload(mucUser);
            return participantJoinedPresence;
        };

        for (const auto& participantJID : jids) {
            stanzaChannel_->onPresenceReceived(mucParticipantJoined(participantJID));
        }
    }

    void testImpromptuChatTitle() {
        // Open a new chat window to a sender.
        MockChatWindow* window = new MockChatWindow();
        impromptuChatSetup(window);
        // After people joined, the title is the list of participant nicknames or names coming from Roster (if nicknames are unavailable)
        CPPUNIT_ASSERT_EQUAL(std::string("bar@test.com, foo@test.com"), manager_->getRecentChats()[0].getTitle());
    }

    void testImpromptuChatWindowTitle() {
        // Open a new chat window to a sender.
        MockChatWindow* window = new MockChatWindow();
        impromptuChatSetup(window);
        // After people joined, the title of chat window is combined of participant nicknames or names coming from Roster (if nicknames are unavailable)
        CPPUNIT_ASSERT_EQUAL(std::string("bar@test.com, foo@test.com"), window->name_);
    }

    void testStandardMUCChatWindowTitle() {
        JID mucJID("mucroom@rooms.test.com");
        std::string nickname = "toodles";

        MockChatWindow* window = new MockChatWindow();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(mucJID, uiEventStream_).Return(window);

        uiEventStream_->send(std::make_shared<JoinMUCUIEvent>(mucJID, boost::optional<std::string>(), nickname));
        CPPUNIT_ASSERT_EQUAL(std::string("mucroom"), window->name_);
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
        if (action.getSoundFilePath()) {
            soundsPlayed_.insert(action.getSoundFilePath().get_value_or(""));
        }
    }

private:
    JID jid_;
    std::unique_ptr<DummyNotifier> notifier_;
    ChatsManager* manager_;
    DummyStanzaChannel* stanzaChannel_;
    IQRouter* iqRouter_;
    EventController* eventController_;
    ChatWindowFactory* chatWindowFactory_;
    JoinMUCWindowFactory* joinMUCWindowFactory_;
    NickResolver* nickResolver_;
    PresenceOracle* presenceOracle_;
    AvatarManager* avatarManager_;
    EventNotifier* eventNotifier_;
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
    DummyTimerFactory* timerFactory_;

};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatsManagerTest);

