/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>
#include <vector>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Disco/CapsInfoGenerator.h>
#include <Swiften/Disco/CapsProvider.h>
#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/Elements/CapsInfo.h>

using namespace Swift;

class EntityCapsManagerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(EntityCapsManagerTest);
        CPPUNIT_TEST(testReceiveKnownHash);
        CPPUNIT_TEST(testReceiveKnownHashTwiceDoesNotTriggerChange);
        CPPUNIT_TEST(testReceiveUnknownHashDoesNotTriggerChange);
        CPPUNIT_TEST(testReceiveUnknownHashAfterKnownHashTriggersChangeAndClearsCaps);
        CPPUNIT_TEST(testReceiveUnavailablePresenceAfterKnownHashTriggersChangeAndClearsCaps);
        CPPUNIT_TEST(testReconnectTriggersChangeAndClearsCaps);
        CPPUNIT_TEST(testHashAvailable);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());

            stanzaChannel = std::unique_ptr<DummyStanzaChannel>(new DummyStanzaChannel());
            capsProvider = std::unique_ptr<DummyCapsProvider>(new DummyCapsProvider());

            user1 = JID("user1@bar.com/bla");
            discoInfo1 = std::make_shared<DiscoInfo>();
            discoInfo1->addFeature("http://swift.im/feature1");
            capsInfo1 = std::make_shared<CapsInfo>(CapsInfoGenerator("http://node1.im", crypto.get()).generateCapsInfo(*discoInfo1.get()));
            capsInfo1alt = std::make_shared<CapsInfo>(CapsInfoGenerator("http://node2.im", crypto.get()).generateCapsInfo(*discoInfo1.get()));
            user2 = JID("user2@foo.com/baz");
            discoInfo2 = std::make_shared<DiscoInfo>();
            discoInfo2->addFeature("http://swift.im/feature2");
            capsInfo2 = std::make_shared<CapsInfo>(CapsInfoGenerator("http://node2.im", crypto.get()).generateCapsInfo(*discoInfo2.get()));
            user3 = JID("user3@foo.com/baz");
            legacyCapsInfo = std::make_shared<CapsInfo>("http://swift.im", "ver1", "");
        }

        void testReceiveKnownHash() {
            std::shared_ptr<EntityCapsManager> testling = createManager();
            capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
            CPPUNIT_ASSERT_EQUAL(discoInfo1, testling->getCaps(user1));
        }

        void testReceiveKnownHashTwiceDoesNotTriggerChange() {
            std::shared_ptr<EntityCapsManager> testling = createManager();
            capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
            sendPresenceWithCaps(user1, capsInfo1);
            changes.clear();

            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
        }

        void testReceiveUnknownHashDoesNotTriggerChange() {
            std::shared_ptr<EntityCapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
        }

        void testHashAvailable() {
            std::shared_ptr<EntityCapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);

            capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
            capsProvider->onCapsAvailable(capsInfo1->getVersion());

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
            CPPUNIT_ASSERT_EQUAL(discoInfo1, testling->getCaps(user1));
        }

        void testReceiveUnknownHashAfterKnownHashTriggersChangeAndClearsCaps() {
            std::shared_ptr<EntityCapsManager> testling = createManager();
            capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
            sendPresenceWithCaps(user1, capsInfo1);
            changes.clear();
            sendPresenceWithCaps(user1, capsInfo2);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
            CPPUNIT_ASSERT(!testling->getCaps(user1));
        }

        void testReceiveUnavailablePresenceAfterKnownHashTriggersChangeAndClearsCaps() {
            std::shared_ptr<EntityCapsManager> testling = createManager();
            capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
            sendPresenceWithCaps(user1, capsInfo1);
            changes.clear();
            sendUnavailablePresence(user1);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
            CPPUNIT_ASSERT(!testling->getCaps(user1));
        }

        void testReconnectTriggersChangeAndClearsCaps() {
            std::shared_ptr<EntityCapsManager> testling = createManager();
            capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
            capsProvider->caps[capsInfo2->getVersion()] = discoInfo2;
            sendPresenceWithCaps(user1, capsInfo1);
            sendPresenceWithCaps(user2, capsInfo2);
            changes.clear();
            stanzaChannel->setAvailable(false);
            stanzaChannel->setAvailable(true);

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
            CPPUNIT_ASSERT(!testling->getCaps(user1));
            CPPUNIT_ASSERT_EQUAL(user2, changes[1]);
            CPPUNIT_ASSERT(!testling->getCaps(user2));
        }

    private:
        std::shared_ptr<EntityCapsManager> createManager() {
            std::shared_ptr<EntityCapsManager> manager(new EntityCapsManager(capsProvider.get(), stanzaChannel.get()));
            manager->onCapsChanged.connect(boost::bind(&EntityCapsManagerTest::handleCapsChanged, this, _1));
            return manager;
        }

        void handleCapsChanged(const JID& jid) {
            changes.push_back(jid);
        }

        void sendPresenceWithCaps(const JID& jid, std::shared_ptr<CapsInfo> caps) {
            std::shared_ptr<Presence> presence(new Presence());
            presence->setFrom(jid);
            presence->addPayload(caps);
            stanzaChannel->onPresenceReceived(presence);
        }

        void sendUnavailablePresence(const JID& jid) {
            std::shared_ptr<Presence> presence(new Presence());
            presence->setFrom(jid);
            presence->setType(Presence::Unavailable);
            stanzaChannel->onPresenceReceived(presence);
        }

    private:
        struct DummyCapsProvider : public CapsProvider {
            virtual DiscoInfo::ref getCaps(const std::string& hash) const {
                std::map<std::string, DiscoInfo::ref>::const_iterator i = caps.find(hash);
                if (i != caps.end()) {
                    return i->second;
                }
                return DiscoInfo::ref();
            }

            std::map<std::string, DiscoInfo::ref> caps;
        };

    private:
        std::unique_ptr<DummyStanzaChannel> stanzaChannel;
        std::unique_ptr<DummyCapsProvider> capsProvider;
        JID user1;
        std::shared_ptr<DiscoInfo> discoInfo1;
        std::shared_ptr<CapsInfo> capsInfo1;
        std::shared_ptr<CapsInfo> capsInfo1alt;
        JID user2;
        std::shared_ptr<DiscoInfo> discoInfo2;
        std::shared_ptr<CapsInfo> capsInfo2;
        std::shared_ptr<CapsInfo> legacyCapsInfo;
        JID user3;
        std::vector<JID> changes;
        std::shared_ptr<CryptoProvider> crypto;
};

CPPUNIT_TEST_SUITE_REGISTRATION(EntityCapsManagerTest);
