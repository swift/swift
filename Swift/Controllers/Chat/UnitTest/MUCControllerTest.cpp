/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/algorithm/string.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <hippomocks.h>

#include <Swiften/Avatars/NullAvatarManager.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Disco/DummyEntityCapsProvider.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/Elements/Thread.h>
#include <Swiften/MUC/MUCBookmarkManager.h>
#include <Swiften/MUC/UnitTest/MockMUC.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Presence/StanzaChannelPresenceSender.h>
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
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
    CPPUNIT_TEST(testSubjectChangeCorrect);
    CPPUNIT_TEST(testSubjectChangeIncorrectA);
    CPPUNIT_TEST(testSubjectChangeIncorrectB);
    CPPUNIT_TEST(testSubjectChangeIncorrectC);
    CPPUNIT_TEST(testHandleOccupantNicknameChanged);
    CPPUNIT_TEST(testHandleOccupantNicknameChangedRoster);
    CPPUNIT_TEST(testHandleChangeSubjectRequest);

    CPPUNIT_TEST(testNonImpromptuMUCWindowTitle);

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        crypto_ = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
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
        xmppRoster_ = new XMPPRosterImpl();
        presenceOracle_ = new PresenceOracle(stanzaChannel_, xmppRoster_);
        presenceSender_ = new StanzaChannelPresenceSender(stanzaChannel_);
        directedPresenceSender_ = new DirectedPresenceSender(presenceSender_);
        uiEventStream_ = new UIEventStream();
        avatarManager_ = new NullAvatarManager();
        TimerFactory* timerFactory = nullptr;
        window_ = new MockChatWindow();
        mucRegistry_ = new MUCRegistry();
        entityCapsProvider_ = new DummyEntityCapsProvider();
        settings_ = new DummySettingsProvider();
        highlightManager_ = new HighlightManager(settings_);
        highlightManager_->resetToDefaultConfiguration();
        muc_ = std::make_shared<MockMUC>(mucJID_);
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(muc_->getJID(), uiEventStream_).Return(window_);
        chatMessageParser_ = std::make_shared<ChatMessageParser>(std::map<std::string, std::string>(), highlightManager_->getConfiguration(), ChatMessageParser::Mode::GroupChat);
        vcardStorage_ = new VCardMemoryStorage(crypto_.get());
        vcardManager_ = new VCardManager(self_, iqRouter_, vcardStorage_);
        nickResolver_ = new NickResolver(self_, xmppRoster_, vcardManager_, mucRegistry_);
        clientBlockListManager_ = new ClientBlockListManager(iqRouter_);
        mucBookmarkManager_ = new MUCBookmarkManager(iqRouter_);
        controller_ = new MUCController (self_, muc_, boost::optional<std::string>(), nick_, stanzaChannel_, iqRouter_, chatWindowFactory_, nickResolver_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory, eventController_, entityCapsProvider_, nullptr, nullptr, mucRegistry_, highlightManager_, clientBlockListManager_, chatMessageParser_, false, nullptr, vcardManager_, mucBookmarkManager_);
    }

    void tearDown() {
        delete controller_;
        delete mucBookmarkManager_;
        delete clientBlockListManager_;
        delete nickResolver_;
        delete vcardManager_;
        delete vcardStorage_;
        delete highlightManager_;
        delete settings_;
        delete entityCapsProvider_;
        delete eventController_;
        delete presenceOracle_;
        delete xmppRoster_;
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

    void joinCompleted() {
        std::string messageBody("test message");
        window_->onSendMessageRequest(messageBody, false);
        std::shared_ptr<Stanza> rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
        Message::ref message = std::dynamic_pointer_cast<Message>(rawStanza);
        CPPUNIT_ASSERT(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
        CPPUNIT_ASSERT(message);
        CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody().get_value_or(""));

        {
            Message::ref message = std::make_shared<Message>();
            message->setType(Message::Groupchat);
            message->setTo(self_);
            message->setFrom(mucJID_.withResource("SomeNickname"));
            message->setID(iqChannel_->getNewIQID());
            message->setSubject("Initial");

            controller_->handleIncomingMessage(std::make_shared<MessageEvent>(message));
        }
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

        // The last message is ignored because self-mention highlights are matched case
        // sensitive against the nickname.
        CPPUNIT_ASSERT_EQUAL((size_t)3, eventController_->getEvents().size());

        message = Message::ref(new Message());
        message->setFrom(JID(muc_->getJID().toString() + "/other3"));
        message->setBody("Hi bert.");
        message->setType(Message::Groupchat);
        controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
        CPPUNIT_ASSERT_EQUAL((size_t)3, eventController_->getEvents().size());

        message = Message::ref(new Message());
        message->setFrom(JID(muc_->getJID().toString() + "/other2"));
        message->setBody("Hi " + boost::to_lower_copy(nick_) + "ie.");
        message->setType(Message::Groupchat);
        controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
        CPPUNIT_ASSERT_EQUAL((size_t)3, eventController_->getEvents().size());
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
        std::shared_ptr<DiscoInfo> features = std::make_shared<DiscoInfo>();
        features->addFeature(DiscoInfo::SecurityLabelsCatalogFeature);
        controller_->setAvailableServerFeatures(features);
        IQ::ref iq = iqChannel_->iqs_[iqChannel_->iqs_.size() - 1];
        SecurityLabelsCatalog::ref labelPayload = std::make_shared<SecurityLabelsCatalog>();
        labelPayload->addItem(label);
        IQ::ref result = IQ::createResult(self_, iq->getID(), labelPayload);
        iqChannel_->onIQReceived(result);
        std::string messageBody("agamemnon");
        window_->onSendMessageRequest(messageBody, false);
        std::shared_ptr<Stanza> rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
        Message::ref message = std::dynamic_pointer_cast<Message>(rawStanza);
        CPPUNIT_ASSERT_EQUAL(iq->getTo(), result->getFrom());
        CPPUNIT_ASSERT(window_->labelsEnabled_);
        CPPUNIT_ASSERT(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
        CPPUNIT_ASSERT(message);
        CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody().get());
        CPPUNIT_ASSERT(!message->getPayload<SecurityLabel>());
    }

    void testMessageWithLabelItem() {
        std::shared_ptr<SecurityLabel> label = std::make_shared<SecurityLabel>();
        label->setLabel("a");
        SecurityLabelsCatalog::Item labelItem;
        labelItem.setSelector("Bob");
        labelItem.setLabel(label);
        window_->label_ = labelItem;
        std::shared_ptr<DiscoInfo> features = std::make_shared<DiscoInfo>();
        features->addFeature(DiscoInfo::SecurityLabelsCatalogFeature);
        controller_->setAvailableServerFeatures(features);
        IQ::ref iq = iqChannel_->iqs_[iqChannel_->iqs_.size() - 1];
        SecurityLabelsCatalog::ref labelPayload = std::make_shared<SecurityLabelsCatalog>();
        labelPayload->addItem(labelItem);
        IQ::ref result = IQ::createResult(self_, iq->getID(), labelPayload);
        iqChannel_->onIQReceived(result);
        std::string messageBody("agamemnon");
        window_->onSendMessageRequest(messageBody, false);
        std::shared_ptr<Stanza> rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
        Message::ref message = std::dynamic_pointer_cast<Message>(rawStanza);
        CPPUNIT_ASSERT_EQUAL(iq->getTo(), result->getFrom());
        CPPUNIT_ASSERT(window_->labelsEnabled_);
        CPPUNIT_ASSERT(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
        CPPUNIT_ASSERT(message);
        CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody().get());
        CPPUNIT_ASSERT_EQUAL(label, message->getPayload<SecurityLabel>());
    }

    void testCorrectMessageWithLabelItem() {
        std::shared_ptr<SecurityLabel> label = std::make_shared<SecurityLabel>();
        label->setLabel("a");
        SecurityLabelsCatalog::Item labelItem;
        labelItem.setSelector("Bob");
        labelItem.setLabel(label);
        std::shared_ptr<SecurityLabel> label2 = std::make_shared<SecurityLabel>();
        label->setLabel("b");
        SecurityLabelsCatalog::Item labelItem2;
        labelItem2.setSelector("Charlie");
        labelItem2.setLabel(label2);
        window_->label_ = labelItem;
        std::shared_ptr<DiscoInfo> features = std::make_shared<DiscoInfo>();
        features->addFeature(DiscoInfo::SecurityLabelsCatalogFeature);
        controller_->setAvailableServerFeatures(features);
        IQ::ref iq = iqChannel_->iqs_[iqChannel_->iqs_.size() - 1];
        SecurityLabelsCatalog::ref labelPayload = std::make_shared<SecurityLabelsCatalog>();
        labelPayload->addItem(labelItem);
        IQ::ref result = IQ::createResult(self_, iq->getID(), labelPayload);
        iqChannel_->onIQReceived(result);
        std::string messageBody("agamemnon");
        window_->onSendMessageRequest(messageBody, false);
        std::shared_ptr<Stanza> rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
        Message::ref message = std::dynamic_pointer_cast<Message>(rawStanza);
        CPPUNIT_ASSERT_EQUAL(iq->getTo(), result->getFrom());
        CPPUNIT_ASSERT(window_->labelsEnabled_);
        CPPUNIT_ASSERT(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
        CPPUNIT_ASSERT(message);
        CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody().get());
        CPPUNIT_ASSERT_EQUAL(label, message->getPayload<SecurityLabel>());
        window_->label_ = labelItem2;
        window_->onSendMessageRequest(messageBody, true);
        rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
        message = std::dynamic_pointer_cast<Message>(rawStanza);
        CPPUNIT_ASSERT_EQUAL(messageBody, message->getBody().get());
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
        for (auto&& occupant : occupants) {
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

        for (const auto& alteration : alterations) {
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

    void testSubjectChangeCorrect() {
        joinCompleted();

        {
            Message::ref message = std::make_shared<Message>();
            message->setType(Message::Groupchat);
            message->setTo(self_);
            message->setFrom(mucJID_.withResource("SomeNickname"));
            message->setID("3FB99C56-7C92-4755-91B0-9C0098BC7AE0");
            message->setSubject("New Room Subject");

            controller_->handleIncomingMessage(std::make_shared<MessageEvent>(message));
            CPPUNIT_ASSERT_EQUAL(std::string("The room subject is now: New Room Subject"), std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(window_->lastAddedSystemMessage_.getParts()[0])->text);
        }
    }

    /*
     * Test that message stanzas with subject element and non-empty body element do not cause a subject change.
     */
    void testSubjectChangeIncorrectA() {
        joinCompleted();

        {
            Message::ref message = std::make_shared<Message>();
            message->setType(Message::Groupchat);
            message->setTo(self_);
            message->setFrom(mucJID_.withResource("SomeNickname"));
            message->setID(iqChannel_->getNewIQID());
            message->setSubject("New Room Subject");
            message->setBody("Some body text that prevents this stanza from being a subject change.");

            controller_->handleIncomingMessage(std::make_shared<MessageEvent>(message));
            CPPUNIT_ASSERT_EQUAL(std::string("Trying to enter room teaparty@rooms.wonderland.lit"), std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(window_->lastAddedSystemMessage_.getParts()[0])->text);
        }
    }

    /*
     * Test that message stanzas with subject element and thread element do not cause a subject change.
     */
    void testSubjectChangeIncorrectB() {
        joinCompleted();

        {
            Message::ref message = std::make_shared<Message>();
            message->setType(Message::Groupchat);
            message->setTo(self_);
            message->setFrom(mucJID_.withResource("SomeNickname"));
            message->setID(iqChannel_->getNewIQID());
            message->setSubject("New Room Subject");
            message->addPayload(std::make_shared<Thread>("Thread that prevents the subject change."));

            controller_->handleIncomingMessage(std::make_shared<MessageEvent>(message));
            CPPUNIT_ASSERT_EQUAL(std::string("Trying to enter room teaparty@rooms.wonderland.lit"), std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(window_->lastAddedSystemMessage_.getParts()[0])->text);
        }
    }

    /*
     * Test that message stanzas with subject element and empty body element do not cause a subject change.
     */
    void testSubjectChangeIncorrectC() {
        joinCompleted();

        {
            Message::ref message = std::make_shared<Message>();
            message->setType(Message::Groupchat);
            message->setTo(self_);
            message->setFrom(mucJID_.withResource("SomeNickname"));
            message->setID(iqChannel_->getNewIQID());
            message->setSubject("New Room Subject");
            message->setBody("");

            controller_->handleIncomingMessage(std::make_shared<MessageEvent>(message));
            CPPUNIT_ASSERT_EQUAL(std::string("Trying to enter room teaparty@rooms.wonderland.lit"), std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(window_->lastAddedSystemMessage_.getParts()[0])->text);
        }
    }

    void testHandleOccupantNicknameChanged() {
        const auto occupantCount = [&](const std::string & nick) {
            auto roster = window_->getRosterModel();
            CPPUNIT_ASSERT(roster != nullptr);
            const auto currentOccupantsJIDs = roster->getJIDs();
            int count = 0;
            for (auto & p : currentOccupantsJIDs) {
                if (p.getResource() == nick) {
                    ++count;
                }
            }
            return count;
        };

        muc_->insertOccupant(MUCOccupant("TestUserOne", MUCOccupant::Participant, MUCOccupant::Owner));
        muc_->insertOccupant(MUCOccupant("TestUserTwo", MUCOccupant::Participant, MUCOccupant::Owner));
        muc_->insertOccupant(MUCOccupant("TestUserThree", MUCOccupant::Participant, MUCOccupant::Owner));

        muc_->onOccupantNicknameChanged("TestUserOne", "TestUserTwo");

        CPPUNIT_ASSERT_EQUAL(0, occupantCount("TestUserOne"));
        CPPUNIT_ASSERT_EQUAL(1, occupantCount("TestUserTwo"));
        CPPUNIT_ASSERT_EQUAL(1, occupantCount("TestUserThree"));
    }

    void testHandleOccupantNicknameChangedRoster() {
        const auto occupantCount = [&](const std::string & nick) {
            auto roster = window_->getRosterModel();
            CPPUNIT_ASSERT(roster != nullptr);
            const auto participants = roster->getGroup("Participants");
            CPPUNIT_ASSERT(participants != nullptr);
            const auto displayedParticipants = participants->getDisplayedChildren();
            int count = 0;
            for (auto & p : displayedParticipants) {
                if (p->getDisplayName() == nick) {
                    ++count;
                }
            }
            return count;
        };

        muc_->insertOccupant(MUCOccupant("TestUserOne", MUCOccupant::Participant, MUCOccupant::Owner));
        muc_->insertOccupant(MUCOccupant("TestUserTwo", MUCOccupant::Participant, MUCOccupant::Owner));
        muc_->insertOccupant(MUCOccupant("TestUserThree", MUCOccupant::Participant, MUCOccupant::Owner));
        CPPUNIT_ASSERT_EQUAL(1, occupantCount("TestUserOne"));
        CPPUNIT_ASSERT_EQUAL(1, occupantCount("TestUserTwo"));
        CPPUNIT_ASSERT_EQUAL(1, occupantCount("TestUserThree"));

        muc_->onOccupantNicknameChanged("TestUserOne", "TestUserTwo");

        CPPUNIT_ASSERT_EQUAL(0, occupantCount("TestUserOne"));
        CPPUNIT_ASSERT_EQUAL(1, occupantCount("TestUserTwo"));
        CPPUNIT_ASSERT_EQUAL(1, occupantCount("TestUserThree"));
    }

    void testRoleAffiliationStatesVerify(const std::map<std::string, MUCOccupant> &occupants) {
        /* verify that the roster is in sync */
        GroupRosterItem* group = window_->getRosterModel()->getRoot();
        for (auto rosterItem : group->getChildren()) {
            GroupRosterItem* child = dynamic_cast<GroupRosterItem*>(rosterItem);
            CPPUNIT_ASSERT(child);
            for (auto childItem : child->getChildren()) {
                ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(childItem);
                CPPUNIT_ASSERT(item);
                std::map<std::string, MUCOccupant>::const_iterator occupant = occupants.find(item->getJID().getResource());
                CPPUNIT_ASSERT(occupant != occupants.end());
                CPPUNIT_ASSERT(item->getMUCRole() == occupant->second.getRole());
                CPPUNIT_ASSERT(item->getMUCAffiliation() == occupant->second.getAffiliation());
            }
        }
    }

    void testHandleChangeSubjectRequest() {
        std::string testStr("New Subject");
        CPPUNIT_ASSERT_EQUAL(std::string(""), muc_->newSubjectSet_);
        window_->onChangeSubjectRequest(testStr);
        CPPUNIT_ASSERT_EQUAL(testStr, muc_->newSubjectSet_);
    }

    void testNonImpromptuMUCWindowTitle() {
        CPPUNIT_ASSERT_EQUAL(muc_->getJID().getNode(), window_->name_);
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
    NickResolver* nickResolver_;
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
    std::shared_ptr<ChatMessageParser> chatMessageParser_;
    std::shared_ptr<CryptoProvider> crypto_;
    VCardManager* vcardManager_;
    VCardMemoryStorage* vcardStorage_;
    ClientBlockListManager* clientBlockListManager_;
    MUCBookmarkManager* mucBookmarkManager_;
    XMPPRoster* xmppRoster_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCControllerTest);

