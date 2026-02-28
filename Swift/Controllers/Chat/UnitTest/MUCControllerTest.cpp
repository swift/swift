/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/algorithm/string.hpp>

#include <memory>

#include <gtest/gtest.h>
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
#include <Swift/Controllers/Chat/Chattables.h>
#include <Swift/Controllers/Chat/MUCController.h>
#include <Swift/Controllers/Chat/UserSearchController.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/DummySettingsProvider.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindowFactory.h>
#include <Swift/Controllers/UnitTest/MockChatWindow.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

// Clang wrongly things that tests for 0 are using 0 as null.
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"

using namespace Swift;

class MUCControllerTest : public ::testing::Test {

    protected:
        void SetUp() {
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
            chattables_ = std::make_unique<Chattables>();
            controller_ = new MUCController (self_, muc_, boost::optional<std::string>(), nick_, stanzaChannel_, iqRouter_, chatWindowFactory_, nickResolver_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory, eventController_, entityCapsProvider_, nullptr, nullptr, mucRegistry_, highlightManager_, clientBlockListManager_, chatMessageParser_, false, nullptr, vcardManager_, mucBookmarkManager_, settings_, *chattables_);
        }

        void TearDown() {
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
            ASSERT_TRUE(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
            ASSERT_TRUE(message);
            ASSERT_EQ(messageBody, message->getBody().get_value_or(""));

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

        void checkEqual(const std::vector<NickJoinPart>& expected, const std::vector<NickJoinPart>& actual) {
            ASSERT_EQ(expected.size(), actual.size());
            for (size_t i = 0; i < expected.size(); i++) {
                ASSERT_EQ(expected[i].nick, actual[i].nick);
                ASSERT_EQ(expected[i].type, actual[i].type);
            }
        }

        JID jidFromOccupant(const MUCOccupant& occupant) {
            return JID(mucJID_.toString()+"/"+occupant.getNick());
        }

        void testRoleAffiliationStatesVerify(const std::map<std::string, MUCOccupant> &occupants) {
            /* verify that the roster is in sync */
            GroupRosterItem* group = window_->getRosterModel()->getRoot();
            for (auto rosterItem : group->getChildren()) {
                GroupRosterItem* child = dynamic_cast<GroupRosterItem*>(rosterItem);
                ASSERT_TRUE(child);
                for (auto childItem : child->getChildren()) {
                    ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(childItem);
                    ASSERT_TRUE(item);
                    std::map<std::string, MUCOccupant>::const_iterator occupant = occupants.find(item->getJID().getResource());
                    ASSERT_TRUE(occupant != occupants.end());
                    ASSERT_TRUE(item->getMUCRole() == occupant->second.getRole());
                    ASSERT_TRUE(item->getMUCAffiliation() == occupant->second.getAffiliation());
                }
            }
        }

        void setMUCSecurityMarking(const std::string& markingValue, const std::string& markingForegroundColorValue, const std::string& markingBackgroundColorValue, const bool includeFormTypeField = true) {
            auto form = std::make_shared<Form>(Form::Type::ResultType);

            if (includeFormTypeField) {
                std::shared_ptr<FormField> formTypeField = std::make_shared<FormField>(FormField::Type::HiddenType, "http://jabber.org/protocol/muc#roominfo");
                formTypeField->setName("FORM_TYPE");
                form->addField(formTypeField);
            }

            auto markingField = std::make_shared<FormField>(FormField::Type::TextSingleType, markingValue);
            auto markingForegroundColorField = std::make_shared<FormField>(FormField::Type::TextSingleType, markingForegroundColorValue);
            auto markingBackgroundColorField = std::make_shared<FormField>(FormField::Type::TextSingleType, markingBackgroundColorValue);

            markingField->setName("x-isode#roominfo_marking");
            markingForegroundColorField->setName("x-isode#roominfo_marking_fg_color");
            markingBackgroundColorField->setName("x-isode#roominfo_marking_bg_color");

            form->addField(markingField);
            form->addField(markingForegroundColorField);
            form->addField(markingBackgroundColorField);

            auto discoInfoRef = std::make_shared<DiscoInfo>();
            discoInfoRef->addExtension(form);

            auto infoResponse = IQ::createResult(self_, mucJID_, "test-id", discoInfoRef);
            iqChannel_->onIQReceived(infoResponse);
        }

        Message::ref createTestMessageWithoutSecurityLabel() {
            auto message = std::make_shared<Message>();
            message->setType(Message::Type::Groupchat);
            message->setID("test-id");
            message->setTo(self_);
            message->setFrom(mucJID_.withResource("TestNickname"));
            message->setBody("Do Not Read This Message");
            return message;
        }

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
        std::unique_ptr<Chattables> chattables_;
};

TEST_F(MUCControllerTest, testAddressedToSelf) {
    finishJoin();
    Message::ref message(new Message());

    message = Message::ref(new Message());
    message->setFrom(JID(muc_->getJID().toString() + "/otherperson"));
    message->setBody("basic " + nick_ + " test.");
    message->setType(Message::Groupchat);
    controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
    ASSERT_EQ(1, eventController_->getEvents().size());

    message = Message::ref(new Message());
    message->setFrom(JID(muc_->getJID().toString() + "/otherperson"));
    message->setBody(nick_ + ": hi there");
    message->setType(Message::Groupchat);
    controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
    ASSERT_EQ(2, eventController_->getEvents().size());

    message->setFrom(JID(muc_->getJID().toString() + "/other"));
    message->setBody("Hi there " + nick_);
    message->setType(Message::Groupchat);
    controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
    ASSERT_EQ(3, eventController_->getEvents().size());

    message = Message::ref(new Message());
    message->setFrom(JID(muc_->getJID().toString() + "/other2"));
    message->setBody("Hi " + boost::to_lower_copy(nick_) + ".");
    message->setType(Message::Groupchat);
    controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));

    // The last message is ignored because self-mention highlights are matched case
    // sensitive against the nickname.
    ASSERT_EQ(3, eventController_->getEvents().size());

    message = Message::ref(new Message());
    message->setFrom(JID(muc_->getJID().toString() + "/other3"));
    message->setBody("Hi bert.");
    message->setType(Message::Groupchat);
    controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
    ASSERT_EQ(3, eventController_->getEvents().size());

    message = Message::ref(new Message());
    message->setFrom(JID(muc_->getJID().toString() + "/other2"));
    message->setBody("Hi " + boost::to_lower_copy(nick_) + "ie.");
    message->setType(Message::Groupchat);
    controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
    ASSERT_EQ(3, eventController_->getEvents().size());
}

TEST_F(MUCControllerTest, testNotAddressedToSelf) {
    finishJoin();
    Message::ref message(new Message());
    message->setFrom(JID(muc_->getJID().toString() + "/other3"));
    message->setBody("Hi there Hatter");
    message->setType(Message::Groupchat);
    controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
    ASSERT_EQ(0, eventController_->getEvents().size());
}

TEST_F(MUCControllerTest, testAddressedToSelfBySelf) {
    finishJoin();
    Message::ref message(new Message());
    message->setFrom(JID(muc_->getJID().toString() + "/" + nick_));
    message->setBody("Hi there " + nick_);
    message->setType(Message::Groupchat);
    controller_->handleIncomingMessage(MessageEvent::ref(new MessageEvent(message)));
    ASSERT_EQ(0, eventController_->getEvents().size());
}

TEST_F(MUCControllerTest, testMessageWithEmptyLabelItem) {
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
    ASSERT_EQ(iq->getTo(), result->getFrom());
    ASSERT_TRUE(window_->labelsEnabled_);
    ASSERT_TRUE(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
    ASSERT_TRUE(message);
    ASSERT_EQ(messageBody, message->getBody().get());
    ASSERT_FALSE(message->getPayload<SecurityLabel>());
}

TEST_F(MUCControllerTest, testMessageWithLabelItem) {
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
    ASSERT_EQ(iq->getTo(), result->getFrom());
    ASSERT_TRUE(window_->labelsEnabled_);
    ASSERT_TRUE(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
    ASSERT_TRUE(message);
    ASSERT_EQ(messageBody, message->getBody().get());
    ASSERT_EQ(label, message->getPayload<SecurityLabel>());
}

TEST_F(MUCControllerTest, testCorrectMessageWithLabelItem) {
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
    ASSERT_EQ(iq->getTo(), result->getFrom());
    ASSERT_TRUE(window_->labelsEnabled_);
    ASSERT_TRUE(stanzaChannel_->isAvailable()); /* Otherwise will prevent sends. */
    ASSERT_TRUE(message);
    ASSERT_EQ(messageBody, message->getBody().get());
    ASSERT_EQ(label, message->getPayload<SecurityLabel>());
    window_->label_ = labelItem2;
    window_->onSendMessageRequest(messageBody, true);
    rawStanza = stanzaChannel_->sentStanzas[stanzaChannel_->sentStanzas.size() - 1];
    message = std::dynamic_pointer_cast<Message>(rawStanza);
    ASSERT_EQ(messageBody, message->getBody().get());
    ASSERT_EQ(label, message->getPayload<SecurityLabel>());
}

TEST_F(MUCControllerTest, testAppendToJoinParts) {
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

TEST_F(MUCControllerTest, testJoinPartStringContructionSimple) {
    std::vector<NickJoinPart> list;
    list.push_back(NickJoinPart("Kev", Join));
    ASSERT_EQ(std::string("Kev has entered the room"), MUCController::generateJoinPartString(list, false));
    list.push_back(NickJoinPart("Remko", Part));
    ASSERT_EQ(std::string("Kev has entered the room and Remko has left the room"), MUCController::generateJoinPartString(list, false));
    list.push_back(NickJoinPart("Bert", Join));
    ASSERT_EQ(std::string("Kev and Bert have entered the room and Remko has left the room"), MUCController::generateJoinPartString(list, false));
    list.push_back(NickJoinPart("Ernie", Join));
    ASSERT_EQ(std::string("Kev, Bert and Ernie have entered the room and Remko has left the room"), MUCController::generateJoinPartString(list, false));
}

TEST_F(MUCControllerTest, testJoinPartStringContructionMixed) {
    std::vector<NickJoinPart> list;
    list.push_back(NickJoinPart("Kev", JoinThenPart));
    ASSERT_EQ(std::string("Kev has entered then left the room"), MUCController::generateJoinPartString(list, false));
    list.push_back(NickJoinPart("Remko", Part));
    ASSERT_EQ(std::string("Remko has left the room and Kev has entered then left the room"), MUCController::generateJoinPartString(list, false));
    list.push_back(NickJoinPart("Bert", PartThenJoin));
    ASSERT_EQ(std::string("Remko has left the room, Kev has entered then left the room and Bert has left then returned to the room"), MUCController::generateJoinPartString(list, false));
    list.push_back(NickJoinPart("Ernie", JoinThenPart));
    ASSERT_EQ(std::string("Remko has left the room, Kev and Ernie have entered then left the room and Bert has left then returned to the room"), MUCController::generateJoinPartString(list, false));
}

TEST_F(MUCControllerTest, testRoleAffiliationStates) {

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
        ASSERT_TRUE(occupant != occupants.end());
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

TEST_F(MUCControllerTest, testSubjectChangeCorrect) {
    joinCompleted();

    {
        Message::ref message = std::make_shared<Message>();
        message->setType(Message::Groupchat);
        message->setTo(self_);
        message->setFrom(mucJID_.withResource("SomeNickname"));
        message->setID("3FB99C56-7C92-4755-91B0-9C0098BC7AE0");
        message->setSubject("New Room Subject");

        controller_->handleIncomingMessage(std::make_shared<MessageEvent>(message));
        ASSERT_EQ(std::string("The room subject is now: New Room Subject"), std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(window_->lastAddedSystemMessage_.getParts()[0])->text);
    }
}

/*
    * Test that message stanzas with subject element and non-empty body element do not cause a subject change.
    */
TEST_F(MUCControllerTest, testSubjectChangeIncorrectA) {
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
        ASSERT_EQ(std::string("Trying to enter room teaparty@rooms.wonderland.lit"), std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(window_->lastAddedSystemMessage_.getParts()[0])->text);
    }
}

/*
    * Test that message stanzas with subject element and thread element do not cause a subject change.
    */
TEST_F(MUCControllerTest, testSubjectChangeIncorrectB) {
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
        ASSERT_EQ(std::string("Trying to enter room teaparty@rooms.wonderland.lit"), std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(window_->lastAddedSystemMessage_.getParts()[0])->text);
    }
}

/*
    * Test that message stanzas with subject element and empty body element do not cause a subject change.
    */
TEST_F(MUCControllerTest, testSubjectChangeIncorrectC) {
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
        ASSERT_EQ(std::string("Trying to enter room teaparty@rooms.wonderland.lit"), std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(window_->lastAddedSystemMessage_.getParts()[0])->text);
    }
}

TEST_F(MUCControllerTest, testHandleOccupantNicknameChanged) {
    const auto occupantCount = [&](const std::string & nick) {
        auto roster = window_->getRosterModel();
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

    ASSERT_EQ(0, occupantCount("TestUserOne"));
    ASSERT_EQ(1, occupantCount("TestUserTwo"));
    ASSERT_EQ(1, occupantCount("TestUserThree"));
}

TEST_F(MUCControllerTest, testHandleOccupantNicknameChangedRoster) {
    const auto occupantCount = [&](const std::string & nick) {
        auto roster = window_->getRosterModel();
        const auto participants = roster->getGroup("Participants");
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
    ASSERT_EQ(1, occupantCount("TestUserOne"));
    ASSERT_EQ(1, occupantCount("TestUserTwo"));
    ASSERT_EQ(1, occupantCount("TestUserThree"));

    muc_->onOccupantNicknameChanged("TestUserOne", "TestUserTwo");

    ASSERT_EQ(0, occupantCount("TestUserOne"));
    ASSERT_EQ(1, occupantCount("TestUserTwo"));
    ASSERT_EQ(1, occupantCount("TestUserThree"));
}

TEST_F(MUCControllerTest, testHandleChangeSubjectRequest) {
    std::string testStr("New Subject");
    ASSERT_EQ(std::string(""), muc_->newSubjectSet_);
    window_->onChangeSubjectRequest(testStr);
    ASSERT_EQ(testStr, muc_->newSubjectSet_);
}

TEST_F(MUCControllerTest, testNonImpromptuMUCWindowTitle) {
    ASSERT_EQ(muc_->getJID().getNode(), window_->name_);
}

TEST_F(MUCControllerTest, testSecurityMarkingRequestCompleteMarking) {
    setMUCSecurityMarking("Test|Highest Possible Security", "Black", "Red", true);

    ASSERT_EQ(std::string("Test|Highest Possible Security"), window_->markingValue_);
    ASSERT_EQ(std::string("Black"), window_->markingForegroundColorValue_);
    ASSERT_EQ(std::string("Red"), window_->markingBackgroundColorValue_);
}

TEST_F(MUCControllerTest, testSecurityMarkingRequestCompleteMarkingWithExtraForm) {
    auto formTypeField = std::make_shared<FormField>(FormField::Type::HiddenType, "http://jabber.org/protocol/muc#roominfo");
    auto markingField = std::make_shared<FormField>(FormField::Type::TextSingleType, "Test|Highest Possible Security");
    auto markingForegroundColorField = std::make_shared<FormField>(FormField::Type::TextSingleType, "Black");
    auto markingBackgroundColorField = std::make_shared<FormField>(FormField::Type::TextSingleType, "Red");
    formTypeField->setName("FORM_TYPE");
    markingField->setName("x-isode#roominfo_marking");
    markingForegroundColorField->setName("x-isode#roominfo_marking_fg_color");
    markingBackgroundColorField->setName("x-isode#roominfo_marking_bg_color");

    auto extraForm = std::make_shared<Form>(Form::Type::ResultType);
    auto form = std::make_shared<Form>(Form::Type::ResultType);
    form->addField(formTypeField);
    form->addField(markingField);
    form->addField(markingForegroundColorField);
    form->addField(markingBackgroundColorField);

    auto discoInfoRef = std::make_shared<DiscoInfo>();
    discoInfoRef->addExtension(extraForm);
    discoInfoRef->addExtension(form);

    auto infoResponse = IQ::createResult(self_, mucJID_, "test-id", discoInfoRef);
    iqChannel_->onIQReceived(infoResponse);
    ASSERT_EQ(std::string("Test|Highest Possible Security"), window_->markingValue_);
    ASSERT_EQ(std::string("Black"), window_->markingForegroundColorValue_);
    ASSERT_EQ(std::string("Red"), window_->markingBackgroundColorValue_);
}

TEST_F(MUCControllerTest, testSecurityMarkingRequestNoColorsInMarking) {
    auto formTypeField = std::make_shared<FormField>(FormField::Type::HiddenType, "http://jabber.org/protocol/muc#roominfo");
    auto markingField = std::make_shared<FormField>(FormField::Type::TextSingleType, "Test|Highest Possible Security");
    auto markingForegroundColorField = std::make_shared<FormField>(FormField::Type::TextSingleType, "");
    auto markingBackgroundColorField = std::make_shared<FormField>(FormField::Type::TextSingleType, "");
    formTypeField->setName("FORM_TYPE");
    markingField->setName("x-isode#roominfo_marking");
    markingForegroundColorField->setName("x-isode#roominfo_marking_fg_color");
    markingBackgroundColorField->setName("x-isode#roominfo_marking_bg_color");

    auto form = std::make_shared<Form>(Form::Type::ResultType);
    form->addField(formTypeField);
    form->addField(markingField);
    form->addField(markingForegroundColorField);
    form->addField(markingBackgroundColorField);

    auto discoInfoRef = std::make_shared<DiscoInfo>();
    discoInfoRef->addExtension(form);

    auto infoResponse = IQ::createResult(self_, mucJID_, "test-id", discoInfoRef);
    iqChannel_->onIQReceived(infoResponse);
    ASSERT_EQ(std::string("Test|Highest Possible Security"), window_->markingValue_);
    ASSERT_EQ(std::string("Black"), window_->markingForegroundColorValue_);
    ASSERT_EQ(std::string("White"), window_->markingBackgroundColorValue_);
}

TEST_F(MUCControllerTest, testSecurityMarkingRequestEmptyMarking) {
    setMUCSecurityMarking("", "", "", true);

    ASSERT_EQ(std::string(""), window_->markingValue_);
    ASSERT_EQ(std::string(""), window_->markingForegroundColorValue_);
    ASSERT_EQ(std::string(""), window_->markingBackgroundColorValue_);
}

TEST_F(MUCControllerTest, testSecurityMarkingRequestWithMarkingNoFormType) {
    setMUCSecurityMarking("Test|Highest Possible Security", "Black", "Red", false);

    ASSERT_EQ(std::string(""), window_->markingValue_);
    ASSERT_EQ(std::string(""), window_->markingForegroundColorValue_);
    ASSERT_EQ(std::string(""), window_->markingBackgroundColorValue_);
}

TEST_F(MUCControllerTest, testSecurityMarkingRequestNoMarking) {
    auto form = std::make_shared<Form>(Form::Type::ResultType);

    auto discoInfoRef = std::make_shared<DiscoInfo>();
    discoInfoRef->addExtension(form);

    auto infoResponse = IQ::createResult(self_, mucJID_, "test-id", discoInfoRef);
    iqChannel_->onIQReceived(infoResponse);
    ASSERT_EQ(std::string(""), window_->markingValue_);
    ASSERT_EQ(std::string(""), window_->markingForegroundColorValue_);
    ASSERT_EQ(std::string(""), window_->markingBackgroundColorValue_);
}

TEST_F(MUCControllerTest, testSecurityMarkingRequestNoForm) {
    auto discoInfoRef = std::make_shared<DiscoInfo>();

    auto infoResponse = IQ::createResult( self_, mucJID_, "test-id", discoInfoRef);
    iqChannel_->onIQReceived(infoResponse);
    ASSERT_EQ(std::string(""), window_->markingValue_);
    ASSERT_EQ(std::string(""), window_->markingForegroundColorValue_);
    ASSERT_EQ(std::string(""), window_->markingBackgroundColorValue_);
}

TEST_F(MUCControllerTest, testSecurityMarkingRequestError) {
    auto errorPayload = std::make_shared<ErrorPayload>(ErrorPayload::Condition::NotAuthorized, ErrorPayload::Type::Auth);

    auto infoResponse = IQ::createResult( self_, mucJID_, "test-id", errorPayload);
    iqChannel_->onIQReceived(infoResponse);
    ASSERT_EQ(std::string(""), window_->markingValue_);
    ASSERT_EQ(std::string(""), window_->markingForegroundColorValue_);
    ASSERT_EQ(std::string(""), window_->markingBackgroundColorValue_);
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_Elision_NoRoomMarkingA) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, true);
    setMUCSecurityMarking("", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("Test|Highest Possible Security");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    auto sentMessageEvent = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent);

    auto storedSecurityLabel = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string("Test|Highest Possible Security"), storedSecurityLabel->getDisplayMarking());
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_Elision_NoRoomMarkingB) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, true);
    setMUCSecurityMarking("", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    auto sentMessageEvent = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent);

    auto storedSecurityLabel = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string(""), storedSecurityLabel->getDisplayMarking());
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_Elision_WithRoomMarkingA) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, true);
    setMUCSecurityMarking("Test|Highest Possible Security", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("Test|Highest Possible Security");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    // Test the first message matching MUC marking. This message SHOULD have a marking

    auto sentMessageEvent1 = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent1);

    auto storedSecurityLabel1 = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel1 == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string("Test|Highest Possible Security"), storedSecurityLabel1->getDisplayMarking());

    // Test a consecutive message matching MUC marking. This message SHOULD NOT have a marking

    auto sentMessageEvent2 = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent2);

    auto storedSecurityLabel2 = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel2 == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string(""), storedSecurityLabel2->getDisplayMarking());
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_Elision_WithRoomMarkingB) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, true);
    setMUCSecurityMarking("Test|Lower Security Marking", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("Test|Highest Possible Security");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    auto sentMessageEvent = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent);

    auto storedSecurityLabel = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string("Test|Highest Possible Security"), storedSecurityLabel->getDisplayMarking());
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_Elision_WithRoomMarkingC) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, true);
    setMUCSecurityMarking("Test|Highest Possible Security", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    auto sentMessageEvent = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent);

    auto storedSecurityLabel = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string("Unmarked"), storedSecurityLabel->getDisplayMarking());
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_NoElision_NoRoomMarkingA) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, false);
    setMUCSecurityMarking("", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("Test|Highest Possible Security");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    auto sentMessageEvent = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent);

    auto storedSecurityLabel = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string("Test|Highest Possible Security"), storedSecurityLabel->getDisplayMarking());
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_NoElision_NoRoomMarkingB) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, false);
    setMUCSecurityMarking("", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    auto sentMessageEvent = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent);

    auto storedSecurityLabel = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string(""), storedSecurityLabel->getDisplayMarking());
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_NoElision_WithRoomMarkingA) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, false);
    setMUCSecurityMarking("Test|Highest Possible Security", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("Test|Highest Possible Security");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    // Test the first message matching MUC marking. This message SHOULD have a marking

    auto sentMessageEvent1 = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent1);

    auto storedSecurityLabel1 = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel1 == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string("Test|Highest Possible Security"), storedSecurityLabel1->getDisplayMarking());

    // Test a consecutive message matching MUC marking. This message SHOULD ALSO have a marking

    auto sentMessageEvent2 = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent2);

    auto storedSecurityLabel2 = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel2 == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string("Test|Highest Possible Security"), storedSecurityLabel2->getDisplayMarking());
}

TEST_F(MUCControllerTest, testSecurityMarkingAddedToMessage_NoElision_WithRoomMarkingB) {
    settings_->storeSetting(SettingConstants::MUC_MARKING_ELISION, false);
    setMUCSecurityMarking("", "Black", "Red");

    auto messageLabel = std::make_shared<SecurityLabel>();
    messageLabel->setDisplayMarking("");

    auto sentMessage = createTestMessageWithoutSecurityLabel();
    sentMessage->addPayload(messageLabel);

    auto sentMessageEvent = std::make_shared<MessageEvent>(sentMessage);
    controller_->handleIncomingMessage(sentMessageEvent);

    auto storedSecurityLabel = window_->lastAddedMessageSecurityLabel_;

    ASSERT_EQ(false, storedSecurityLabel == nullptr);
    // This is the potentially altered security label that is displayed on the screen
    ASSERT_EQ(std::string(""), storedSecurityLabel->getDisplayMarking());
}
