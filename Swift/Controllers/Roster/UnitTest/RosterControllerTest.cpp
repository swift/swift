/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Avatars/NullAvatarManager.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/DummyNickManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Client/MemoryStorages.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Disco/CapsInfoGenerator.h>
#include <Swiften/Disco/CapsManager.h>
#include <Swiften/Disco/CapsProvider.h>
#include <Swiften/Disco/ClientDiscoManager.h>
#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Presence/SubscriptionManager.h>
#include <Swiften/Queries/DummyIQChannel.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Roster/RosterController.h>
#include <Swift/Controllers/Settings/DummySettingsProvider.h>
#include <Swift/Controllers/UIEvents/RenameRosterItemUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UnitTest/MockMainWindowFactory.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

using namespace Swift;

class DummyCapsProvider : public CapsProvider {
        DiscoInfo::ref getCaps(const std::string&) const {return DiscoInfo::ref(new DiscoInfo());}
};

class RosterControllerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(RosterControllerTest);
        CPPUNIT_TEST(testAdd);
        CPPUNIT_TEST(testAddSubscription);
        CPPUNIT_TEST(testReceiveRename);
        CPPUNIT_TEST(testReceiveRegroup);
        CPPUNIT_TEST(testSendRename);
        CPPUNIT_TEST(testPresence);
        CPPUNIT_TEST(testHighestPresence);
        CPPUNIT_TEST(testNotHighestPresence);
        CPPUNIT_TEST(testUnavailablePresence);
        CPPUNIT_TEST(testRemoveResultsInUnavailablePresence);
        CPPUNIT_TEST(testOwnContactInRosterPresence);
        CPPUNIT_TEST(testMultiResourceFileTransferFeature);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            jid_ = JID("testjid@swift.im/swift");
            xmppRoster_ = new XMPPRosterImpl();
            avatarManager_ = new NullAvatarManager();
            mainWindowFactory_ = new MockMainWindowFactory();
            mucRegistry_ = new MUCRegistry();
            crypto_ = PlatformCryptoProvider::create();
            storages_ = std::unique_ptr<MemoryStorages>(new MemoryStorages(crypto_));
            nickResolver_ = new NickResolver(jid_.toBare(), xmppRoster_, nullptr, mucRegistry_);
            channel_ = new DummyIQChannel();
            router_ = new IQRouter(channel_);
            stanzaChannel_ = new DummyStanzaChannel();
            presenceOracle_ = new PresenceOracle(stanzaChannel_, xmppRoster_);
            subscriptionManager_ = new SubscriptionManager(stanzaChannel_);
            eventController_ = new EventController();
            uiEventStream_ = new UIEventStream();
            settings_ = new DummySettingsProvider();
            nickManager_ = new DummyNickManager();
            capsManager_ = std::unique_ptr<CapsManager>(new CapsManager(storages_->getCapsStorage(), stanzaChannel_, router_, crypto_));
            entityCapsManager_ = new EntityCapsManager(capsManager_.get(), stanzaChannel_);
            jingleSessionManager_ = new JingleSessionManager(router_);

            clientBlockListManager_ = new ClientBlockListManager(router_);
            vcardStorage_ = new VCardMemoryStorage(crypto_);
            vcardManager_ = new VCardManager(jid_, router_, vcardStorage_);
            rosterController_ = new RosterController(jid_, xmppRoster_, avatarManager_, mainWindowFactory_, nickManager_, nickResolver_, presenceOracle_, subscriptionManager_, eventController_, uiEventStream_, router_, settings_, entityCapsManager_, clientBlockListManager_, vcardManager_);
            mainWindow_ = mainWindowFactory_->last;
            capsInfoGenerator_ = std::unique_ptr<CapsInfoGenerator>(new CapsInfoGenerator("", crypto_));
        }

        void tearDown() {
            delete rosterController_;
            delete vcardManager_;
            delete vcardStorage_;
            delete crypto_;
            delete clientBlockListManager_;
            delete jingleSessionManager_;
            delete entityCapsManager_;
            delete nickManager_;
            delete nickResolver_;
            delete mucRegistry_;
            delete mainWindowFactory_;
            delete avatarManager_;
            delete router_;
            delete channel_;
            delete eventController_;
            delete subscriptionManager_;
            delete presenceOracle_;
            delete stanzaChannel_;
            delete uiEventStream_;
            delete settings_;
            delete xmppRoster_;
        }

    GroupRosterItem* groupChild(size_t i) {
        return dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[i]);
    }

    JID withResource(const JID& jid, const std::string& resource) {
        return JID(jid.toBare().toString() + "/" + resource);
    }

    void testPresence() {
        std::vector<std::string> groups;
        groups.push_back("testGroup1");
        groups.push_back("testGroup2");
        JID from("test@testdomain.com");
        xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);
        Presence::ref presence(new Presence());
        presence->setFrom(withResource(from, "bob"));
        presence->setPriority(2);
        presence->setStatus("So totally here");
        stanzaChannel_->onPresenceReceived(presence);
        ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
        CPPUNIT_ASSERT(item);
        CPPUNIT_ASSERT_EQUAL(presence->getStatus(), item->getStatusText());
        ContactRosterItem* item2 = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[1])->getChildren()[0]);
        CPPUNIT_ASSERT(item2);
        CPPUNIT_ASSERT_EQUAL(presence->getStatus(), item2->getStatusText());
    }

    void testHighestPresence() {
        std::vector<std::string> groups;
        groups.push_back("testGroup1");
        JID from("test@testdomain.com");
        xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);
        Presence::ref lowPresence(new Presence());
        lowPresence->setFrom(withResource(from, "bob"));
        lowPresence->setPriority(2);
        lowPresence->setStatus("Not here");
        Presence::ref highPresence(new Presence());
        highPresence->setFrom(withResource(from, "bert"));
        highPresence->setPriority(10);
        highPresence->setStatus("So totally here");
        stanzaChannel_->onPresenceReceived(lowPresence);
        stanzaChannel_->onPresenceReceived(highPresence);
        ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
        CPPUNIT_ASSERT(item);
        CPPUNIT_ASSERT_EQUAL(highPresence->getStatus(), item->getStatusText());
    }

    void testNotHighestPresence() {
        std::vector<std::string> groups;
        groups.push_back("testGroup1");
        JID from("test@testdomain.com");
        xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);
        Presence::ref lowPresence(new Presence());
        lowPresence->setFrom(withResource(from, "bob"));
        lowPresence->setPriority(2);
        lowPresence->setStatus("Not here");
        Presence::ref highPresence(new Presence());
        highPresence->setFrom(withResource(from, "bert"));
        highPresence->setPriority(10);
        highPresence->setStatus("So totally here");
        stanzaChannel_->onPresenceReceived(highPresence);
        stanzaChannel_->onPresenceReceived(lowPresence);
        ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
        CPPUNIT_ASSERT(item);
        CPPUNIT_ASSERT_EQUAL(highPresence->getStatus(), item->getStatusText());
    }

    void testUnavailablePresence() {
        std::vector<std::string> groups;
        groups.push_back("testGroup1");
        JID from("test@testdomain.com");
        xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);

        Presence::ref lowPresence(new Presence());
        lowPresence->setFrom(withResource(from, "bob"));
        lowPresence->setPriority(2);
        lowPresence->setShow(StatusShow::Away);
        lowPresence->setStatus("Not here");
        Presence::ref lowPresenceOffline(new Presence());
        lowPresenceOffline->setFrom(withResource(from, "bob"));
        lowPresenceOffline->setStatus("Signing out");
        lowPresenceOffline->setType(Presence::Unavailable);

        Presence::ref highPresence(new Presence());
        highPresence->setFrom(withResource(from, "bert"));
        highPresence->setPriority(10);
        highPresence->setStatus("So totally here");
        Presence::ref highPresenceOffline(new Presence());
        highPresenceOffline->setFrom(withResource(from, "bert"));
        highPresenceOffline->setType(Presence::Unavailable);

        stanzaChannel_->onPresenceReceived(lowPresence);
        Presence::ref accountPresence = presenceOracle_->getAccountPresence(from);
        CPPUNIT_ASSERT_EQUAL(StatusShow::Away, accountPresence->getShow());

        stanzaChannel_->onPresenceReceived(highPresence);
        accountPresence = presenceOracle_->getAccountPresence(from);
        CPPUNIT_ASSERT_EQUAL(StatusShow::Online, accountPresence->getShow());

        stanzaChannel_->onPresenceReceived(highPresenceOffline);

        // After this, the roster should show the low presence.
        ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
        CPPUNIT_ASSERT(item);

        Presence::ref low = presenceOracle_->getAccountPresence(from);

        CPPUNIT_ASSERT_EQUAL(Presence::Available, low->getType());
        CPPUNIT_ASSERT_EQUAL(lowPresence->getStatus(), low->getStatus());
        CPPUNIT_ASSERT_EQUAL(lowPresence->getShow(), item->getStatusShow());
        CPPUNIT_ASSERT_EQUAL(lowPresence->getStatus(), item->getStatusText());
        stanzaChannel_->onPresenceReceived(lowPresenceOffline);
        item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
        CPPUNIT_ASSERT(item);
        /* A verification that if the test fails, it's the RosterController, not the PresenceOracle. */
        low = presenceOracle_->getHighestPriorityPresence(from);
        CPPUNIT_ASSERT_EQUAL(Presence::Unavailable, low->getType());
        CPPUNIT_ASSERT_EQUAL(lowPresenceOffline->getStatus(), low->getStatus());
        CPPUNIT_ASSERT_EQUAL(StatusShow::None, item->getStatusShow());
        CPPUNIT_ASSERT_EQUAL(lowPresenceOffline->getStatus(), item->getStatusText());
    }

        void testAdd() {
            std::vector<std::string> groups;
            groups.push_back("testGroup1");
            groups.push_back("testGroup2");
            xmppRoster_->addContact(JID("test@testdomain.com/bob"), "name", groups, RosterItemPayload::Both);

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(getUIRosterChildren().size()));
            //CPPUNIT_ASSERT_EQUAL(std::string("Bob"), xmppRoster_->getNameForJID(JID("foo@bar.com")));
        }

         void testAddSubscription() {
            std::vector<std::string> groups;
            JID jid("test@testdomain.com");
            xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::None);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(getUIRosterChildren().size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
            xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::To);
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(getUIRosterChildren().size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));

            xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::Both);
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(getUIRosterChildren().size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));

        }

        void testReceiveRename() {
            std::vector<std::string> groups;
            JID jid("test@testdomain.com");
            xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::Both);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(getUIRosterChildren().size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("name"), groupChild(0)->getChildren()[0]->getDisplayName());
            xmppRoster_->addContact(jid, "NewName", groups, RosterItemPayload::Both);
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(getUIRosterChildren().size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("NewName"), groupChild(0)->getChildren()[0]->getDisplayName());
        }

    void testReceiveRegroup() {
        std::vector<std::string> oldGroups;
        std::vector<std::string> newGroups;
        newGroups.push_back("A Group");
        std::vector<std::string> newestGroups;
        newestGroups.push_back("Best Group");
        JID jid("test@testdomain.com");
        xmppRoster_->addContact(jid, "", oldGroups, RosterItemPayload::Both);

        CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(getUIRosterChildren().size()));
        CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
        CPPUNIT_ASSERT_EQUAL(jid.toString(), groupChild(0)->getChildren()[0]->getDisplayName());

        xmppRoster_->addContact(jid, "new name", newGroups, RosterItemPayload::Both);
        CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(getUIRosterChildren().size()));
        CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
        CPPUNIT_ASSERT_EQUAL(std::string("new name"), groupChild(0)->getChildren()[0]->getDisplayName());
        CPPUNIT_ASSERT_EQUAL(std::string("A Group"), groupChild(0)->getDisplayName());

        xmppRoster_->addContact(jid, "new name", newestGroups, RosterItemPayload::Both);
        CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(getUIRosterChildren().size()));
        CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
        CPPUNIT_ASSERT_EQUAL(std::string("new name"), groupChild(0)->getChildren()[0]->getDisplayName());
        CPPUNIT_ASSERT_EQUAL(std::string("Best Group"), groupChild(0)->getDisplayName());
    }

        void testSendRename() {
            JID jid("testling@wonderland.lit");
            std::vector<std::string> groups;
            groups.push_back("Friends");
            groups.push_back("Enemies");
            xmppRoster_->addContact(jid, "Bob", groups, RosterItemPayload::From);
            CPPUNIT_ASSERT_EQUAL(groups.size(), xmppRoster_->getGroupsForJID(jid).size());
            uiEventStream_->send(std::make_shared<RenameRosterItemUIEvent>(jid, "Robert"));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), channel_->iqs_.size());
            CPPUNIT_ASSERT_EQUAL(IQ::Set, channel_->iqs_[0]->getType());
            std::shared_ptr<RosterPayload> payload = channel_->iqs_[0]->getPayload<RosterPayload>();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), payload->getItems().size());
            RosterItemPayload item = payload->getItems()[0];
            CPPUNIT_ASSERT_EQUAL(jid, item.getJID());
            CPPUNIT_ASSERT_EQUAL(std::string("Robert"), item.getName());

            CPPUNIT_ASSERT_EQUAL(groups.size(), item.getGroups().size());
            assertVectorsEqual(groups, item.getGroups(), __LINE__);
        }

        void testRemoveResultsInUnavailablePresence() {
            std::vector<std::string> groups;
            groups.push_back("testGroup1");
            JID from("test@testdomain.com");
            xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);
            Presence::ref lowPresence(new Presence());
            lowPresence->setFrom(withResource(from, "bob"));
            lowPresence->setPriority(2);
            lowPresence->setStatus("Not here");
            Presence::ref highPresence(new Presence());
            highPresence->setFrom(withResource(from, "bert"));
            highPresence->setPriority(10);
            highPresence->setStatus("So totally here");
            stanzaChannel_->onPresenceReceived(highPresence);
            stanzaChannel_->onPresenceReceived(lowPresence);

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), presenceOracle_->getAllPresence("test@testdomain.com").size());

            xmppRoster_->onJIDRemoved(JID("test@testdomain.com"));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), presenceOracle_->getAllPresence("test@testdomain.com").size());
            CPPUNIT_ASSERT_EQUAL(Presence::Unavailable, presenceOracle_->getAllPresence("test@testdomain.com")[0]->getType());
        }

        void testOwnContactInRosterPresence() {
            std::vector<std::string> groups;
            groups.push_back("testGroup1");
            groups.push_back("testGroup2");
            JID from = jid_;
            xmppRoster_->addContact(from.toBare(), "name", groups, RosterItemPayload::Both);
            Presence::ref presence(new Presence());
            presence->setFrom(from);
            presence->setPriority(2);
            presence->setStatus("So totally here");
            stanzaChannel_->onPresenceReceived(presence);
            ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
            CPPUNIT_ASSERT(item);
            CPPUNIT_ASSERT_EQUAL(presence->getStatus(), item->getStatusText());
            ContactRosterItem* item2 = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[1])->getChildren()[0]);
            CPPUNIT_ASSERT(item2);
            CPPUNIT_ASSERT_EQUAL(presence->getStatus(), item2->getStatusText());
        }

         // This tests a scenario of a contact having a resource supporting Jingle File Transfer and
        // one resource not supporting it, and the contact features being set correctly.
        void testMultiResourceFileTransferFeature() {
            JID contact("test@testdomain.com");
            xmppRoster_->addContact(contact, "Name", {}, RosterItemPayload::Both);

            auto sendPresenceAndAnswerCaps = [=](const JID& from, const DiscoInfo& discoInfo) {
                auto capsInfo = capsInfoGenerator_->generateCapsInfo(discoInfo);

                auto ftClientPresence = std::make_shared<Presence>();
                ftClientPresence->setFrom(from);
                ftClientPresence->setPriority(0);
                ftClientPresence->setShow(StatusShow::Online);
                ftClientPresence->addPayload(std::make_shared<CapsInfo>(capsInfo));
                stanzaChannel_->onPresenceReceived(ftClientPresence);

                // disco reply
                auto discoRequest = channel_->iqs_.back();
                CPPUNIT_ASSERT(discoRequest);
                auto discoReply = IQ::createResult(discoRequest->getFrom(), ftClientPresence->getFrom(), discoRequest->getID(), std::make_shared<DiscoInfo>(discoInfo));
                channel_->onIQReceived(discoReply);
            };

            auto ftDiscoInfo = DiscoInfo();
            ftDiscoInfo.addFeature(DiscoInfo::JingleFeature);
            ftDiscoInfo.addFeature(DiscoInfo::JingleFTFeature);
            ftDiscoInfo.addFeature(DiscoInfo::JingleTransportsIBBFeature);

            sendPresenceAndAnswerCaps(contact.withResource("ft-supported"), ftDiscoInfo);

            auto* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
            CPPUNIT_ASSERT(item);
            CPPUNIT_ASSERT_EQUAL(contact, item->getJID());
            CPPUNIT_ASSERT_EQUAL(true, item->supportsFeature(ContactRosterItem::FileTransferFeature));

            sendPresenceAndAnswerCaps(contact.withResource("ft-unsupported"), DiscoInfo());

            item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
            CPPUNIT_ASSERT(item);
            CPPUNIT_ASSERT_EQUAL(contact, item->getJID());
            CPPUNIT_ASSERT_EQUAL(true, item->supportsFeature(ContactRosterItem::FileTransferFeature));

            auto unavailablePresence = std::make_shared<Presence>();
            unavailablePresence->setFrom(contact.withResource("ft-unsupported"));
            unavailablePresence->setPriority(0);
            unavailablePresence->setType(Presence::Unavailable);
            stanzaChannel_->onPresenceReceived(unavailablePresence);

            item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
            CPPUNIT_ASSERT(item);
            CPPUNIT_ASSERT_EQUAL(contact, item->getJID());
            CPPUNIT_ASSERT_EQUAL(true, item->supportsFeature(ContactRosterItem::FileTransferFeature));

            unavailablePresence = std::make_shared<Presence>();
            unavailablePresence->setFrom(contact.withResource("ft-supported"));
            unavailablePresence->setPriority(0);
            unavailablePresence->setType(Presence::Unavailable);
            stanzaChannel_->onPresenceReceived(unavailablePresence);

            item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(getUIRosterChildren()[0])->getChildren()[0]);
            CPPUNIT_ASSERT(item);
            CPPUNIT_ASSERT_EQUAL(contact, item->getJID());
            CPPUNIT_ASSERT_EQUAL(false, item->supportsFeature(ContactRosterItem::FileTransferFeature));
        }

        void assertVectorsEqual(const std::vector<std::string>& v1, const std::vector<std::string>& v2, int line) {
            for (const auto& entry : v1) {
                if (std::find(v2.begin(), v2.end(), entry) == v2.end()) {
                    std::stringstream stream;
                    stream <<    "Couldn't find " << entry << " in v2 (line " << line << ")";
                    CPPUNIT_FAIL(stream.str());
                }
            }
        }

        const std::vector<RosterItem*>& getUIRosterChildren() const {
            return mainWindow_->roster->getRoot()->getChildren();
        }

    private:
        JID jid_;
        std::unique_ptr<MemoryStorages> storages_;
        XMPPRosterImpl* xmppRoster_;
        MUCRegistry* mucRegistry_;
        AvatarManager* avatarManager_;
        MockMainWindowFactory* mainWindowFactory_;
        NickManager* nickManager_;
        NickResolver* nickResolver_;
        RosterController* rosterController_;
        DummyIQChannel* channel_;
        DummyStanzaChannel* stanzaChannel_;
        IQRouter* router_;
        PresenceOracle* presenceOracle_;
        SubscriptionManager* subscriptionManager_;
        EventController* eventController_;
        UIEventStream* uiEventStream_;
        MockMainWindow* mainWindow_;
        DummySettingsProvider* settings_;
        std::unique_ptr<CapsManager> capsManager_;
        EntityCapsManager* entityCapsManager_;
        JingleSessionManager* jingleSessionManager_;
        ClientBlockListManager* clientBlockListManager_;
        CryptoProvider* crypto_;
        VCardStorage* vcardStorage_;
        VCardManager* vcardManager_;
        std::unique_ptr<CapsInfoGenerator> capsInfoGenerator_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterControllerTest);
