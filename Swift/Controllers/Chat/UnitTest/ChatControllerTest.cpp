/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>
#include <string>

#include <gtest/gtest.h>
#include <hippomocks.h>

#include <Swiften/Avatars/NullAvatarManager.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Disco/DummyEntityCapsProvider.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>

#include <Swift/Controllers/Chat/AutoAcceptMUCInviteDecider.h>
#include <Swift/Controllers/Chat/ChatController.h>
#include <Swift/Controllers/Chat/ChatMessageParser.h>
#include <Swift/Controllers/Chat/Chattables.h>
#include <Swift/Controllers/Settings/DummySettingsProvider.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/UnitTest/MockChatWindow.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

using namespace Swift;

/**
 * Most of the ChatController tests are in ChatsManagerTest.
 * New tests related with ChatController should be added here,
 * and old tests should be migrated when possible.
 */

class ExtendedChatController : public ChatController {
public:
    ExtendedChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, TimerFactory* timerFactory, EventController* eventController, EntityCapsProvider* entityCapsProvider, bool userWantsReceipts, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, ClientBlockListManager* clientBlockListManager, std::shared_ptr<ChatMessageParser> chatMessageParser, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider, SettingsProvider* settings, Chattables& chattables) :
        ChatController(self, stanzaChannel, iqRouter, chatWindowFactory, contact, nickResolver, presenceOracle, avatarManager, isInMUC, useDelayForLatency, eventStream, timerFactory, eventController, entityCapsProvider, userWantsReceipts, historyController, mucRegistry, highlightManager, clientBlockListManager, chatMessageParser, autoAcceptMUCInviteDecider, settings, chattables) {
    }

    std::map<std::shared_ptr<Stanza>, std::string> getUnackedStanzas() { return unackedStanzas_; }
    std::map<std::string, std::shared_ptr<Stanza>> getFailedStanzas() { return failedStanzas_; }
};

class ChatControllerTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        self_ = JID("alice@wonderland.lit");
        other_ = JID("whiterabbit@wonderland.lit");
        stanzaChannel_ = new DummyStanzaChannel();
        iqChannel_ = new DummyIQChannel();
        iqRouter_ = new IQRouter(iqChannel_);
        eventController_ = new EventController();
        xmppRoster_ = new XMPPRosterImpl();
        vCardManager_ = new VCardManager(self_, iqRouter_, vCardMemoryStorage_);
        mucRegistry_ = new MUCRegistry();
        nickResolver_ = new NickResolver(self_, xmppRoster_, vCardManager_, mucRegistry_);
        presenceOracle_ = new PresenceOracle(stanzaChannel_, xmppRoster_);
        avatarManager_ = new NullAvatarManager();
        uiEventStream_ = new UIEventStream();
        timerFactory_ = new DummyTimerFactory();
        entityCapsProvider_ = new DummyEntityCapsProvider();
        settings_ = new DummySettingsProvider();
        highlightManager_ = new HighlightManager(settings_);
        highlightManager_->resetToDefaultConfiguration();
        clientBlockListManager_ = new ClientBlockListManager(iqRouter_);
        autoAcceptMUCInviteDecider_ = new AutoAcceptMUCInviteDecider(self_.getDomain(), xmppRoster_, settings_);
        chatMessageParser_ = std::make_shared<ChatMessageParser>(std::map<std::string, std::string>(), highlightManager_->getConfiguration(), ChatMessageParser::Mode::GroupChat);
        mocks_ = new MockRepository();
        window_ = new MockChatWindow();
        chatWindowFactory_ = mocks_->InterfaceMock<ChatWindowFactory>();
        mocks_->ExpectCall(chatWindowFactory_, ChatWindowFactory::createChatWindow).With(other_, uiEventStream_).Return(window_);
        chattables_ = std::make_unique<Chattables>();

        controller_ = new  ExtendedChatController(self_, stanzaChannel_, iqRouter_, chatWindowFactory_, other_, nickResolver_, presenceOracle_, avatarManager_, false, false, uiEventStream_, timerFactory_, eventController_, entityCapsProvider_, false, nullptr, mucRegistry_, highlightManager_, clientBlockListManager_, chatMessageParser_, nullptr, settings_, *chattables_);
    }
    virtual void TearDown() {
        delete controller_;
        chattables_.reset();
        chatMessageParser_.reset();
        delete autoAcceptMUCInviteDecider_;
        delete clientBlockListManager_;
        delete highlightManager_;
        delete settings_;
        delete entityCapsProvider_;
        delete timerFactory_;
        delete uiEventStream_;
        delete avatarManager_;
        delete presenceOracle_;
        delete nickResolver_;
        delete mucRegistry_;
        delete vCardManager_;
        delete xmppRoster_;
        delete eventController_;
        delete iqRouter_;
        delete iqChannel_;
        delete stanzaChannel_;
    }

    JID self_, other_;
    AvatarManager* avatarManager_ = nullptr;
    ExtendedChatController* controller_ = nullptr;
    ChatWindowFactory* chatWindowFactory_;
    ClientBlockListManager* clientBlockListManager_;
    EventController* eventController_ = nullptr;
    EntityCapsProvider* entityCapsProvider_ = nullptr;
    IQChannel* iqChannel_ = nullptr;
    IQRouter* iqRouter_ = nullptr;
    MockRepository* mocks_;
    MockChatWindow* window_;
    MUCRegistry* mucRegistry_ = nullptr;
    NickResolver* nickResolver_ = nullptr;
    PresenceOracle* presenceOracle_ = nullptr;
    DummyStanzaChannel* stanzaChannel_ = nullptr;
    TimerFactory* timerFactory_;
    XMPPRosterImpl* xmppRoster_ = nullptr;
    UIEventStream* uiEventStream_;
    VCardManager* vCardManager_ = nullptr;
    VCardMemoryStorage* vCardMemoryStorage_ = nullptr;
    DummySettingsProvider* settings_;
    HighlightManager* highlightManager_;
    std::shared_ptr<ChatMessageParser> chatMessageParser_;
    AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider_;
    std::unique_ptr<Chattables> chattables_;

};

TEST_F(ChatControllerTest, testResendMessage) {
    std::string msgBody("TestMsg");
    stanzaChannel_->setStreamManagementEnabled(true);
    window_->onSendMessageRequest(msgBody, false);
    {
        auto failedStanzas = controller_->getFailedStanzas();
        auto unackedStanzas = controller_->getUnackedStanzas();
        ASSERT_EQ(failedStanzas.size(), 0);
        ASSERT_EQ(unackedStanzas.size(), 1);
    }
    //Disconnecting to fail the stanza
    controller_->setOnline(false);
    controller_->setOnline(true);
    {
        auto failedStanzas = controller_->getFailedStanzas();
        auto unackedStanzas = controller_->getUnackedStanzas();
        ASSERT_EQ(failedStanzas.size(), 1);
        ASSERT_EQ(unackedStanzas.size(), 0);
    }
    window_->onResendMessageRequest("id");
    {
        auto failedStanzas = controller_->getFailedStanzas();
        auto unackedStanzas = controller_->getUnackedStanzas();
        ASSERT_EQ(failedStanzas.size(), 0);
        ASSERT_EQ(unackedStanzas.size(), 1);
    }
}
