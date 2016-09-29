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
#include <Swiften/Disco/CapsManager.h>
#include <Swiften/Disco/CapsMemoryStorage.h>
#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class CapsManagerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CapsManagerTest);
        CPPUNIT_TEST(testReceiveNewHashRequestsDisco);
        CPPUNIT_TEST(testReceiveSameHashDoesNotRequestDisco);
        CPPUNIT_TEST(testReceiveLegacyCapsDoesNotRequestDisco);
        CPPUNIT_TEST(testReceiveSameHashFromSameUserAfterFailedDiscoDoesNotRequestDisco);
        CPPUNIT_TEST(testReceiveSameHashFromDifferentUserAfterFailedDiscoRequestsDisco);
        CPPUNIT_TEST(testReceiveSameHashFromDifferentUserAfterIncorrectVerificationRequestsDisco);
        CPPUNIT_TEST(testReceiveDifferentHashFromSameUserAfterFailedDiscoDoesNotRequestDisco);
        CPPUNIT_TEST(testReceiveSameHashAfterSuccesfulDiscoDoesNotRequestDisco);
        CPPUNIT_TEST(testReceiveSuccesfulDiscoStoresCaps);
        CPPUNIT_TEST(testReceiveIncorrectVerificationDiscoDoesNotStoreCaps);
        CPPUNIT_TEST(testReceiveFailingDiscoFallsBack);
        CPPUNIT_TEST(testReceiveNoDiscoFallsBack);
        CPPUNIT_TEST(testReceiveFailingFallbackDiscoFallsBack);
        CPPUNIT_TEST(testReceiveSameHashFromFailingUserAfterReconnectRequestsDisco);
        CPPUNIT_TEST(testReconnectResetsFallback);
        CPPUNIT_TEST(testReconnectResetsRequests);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
            stanzaChannel = std::unique_ptr<DummyStanzaChannel>(new DummyStanzaChannel());
            iqRouter = std::unique_ptr<IQRouter>(new IQRouter(stanzaChannel.get()));
            storage = std::unique_ptr<CapsMemoryStorage>(new CapsMemoryStorage());
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

        void tearDown() {
            iqRouter.reset();
        }

        void testReceiveNewHashRequestsDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user1, IQ::Get));
            std::shared_ptr<DiscoInfo> discoInfo(stanzaChannel->sentStanzas[0]->getPayload<DiscoInfo>());
            CPPUNIT_ASSERT(discoInfo);
            CPPUNIT_ASSERT_EQUAL("http://node1.im#" + capsInfo1->getVersion(), discoInfo->getNode());
        }

        void testReceiveSameHashDoesNotRequestDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            stanzaChannel->sentStanzas.clear();
            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
        }

        void testReceiveLegacyCapsDoesNotRequestDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, legacyCapsInfo);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
        }

        void testReceiveSameHashAfterSuccesfulDiscoDoesNotRequestDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendDiscoInfoResult(discoInfo1);

            stanzaChannel->sentStanzas.clear();
            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
        }

        void testReceiveSameHashFromSameUserAfterFailedDiscoDoesNotRequestDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

            stanzaChannel->sentStanzas.clear();
            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
        }

        void testReceiveSameHashFromSameUserAfterIncorrectVerificationDoesNotRequestDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendDiscoInfoResult(discoInfo2);

            stanzaChannel->sentStanzas.clear();
            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
        }

        void testReceiveSameHashFromDifferentUserAfterFailedDiscoRequestsDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID()));

            stanzaChannel->sentStanzas.clear();
            sendPresenceWithCaps(user2, capsInfo1);
            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user2, IQ::Get));
        }

        void testReceiveSameHashFromDifferentUserAfterIncorrectVerificationRequestsDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendDiscoInfoResult(discoInfo2);

            stanzaChannel->sentStanzas.clear();
            sendPresenceWithCaps(user2, capsInfo1);
            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user2, IQ::Get));
        }

        void testReceiveDifferentHashFromSameUserAfterFailedDiscoDoesNotRequestDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

            stanzaChannel->sentStanzas.clear();
            sendPresenceWithCaps(user1, capsInfo2);

            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user1, IQ::Get));
        }

        void testReceiveSuccesfulDiscoStoresCaps() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendDiscoInfoResult(discoInfo1);

            std::shared_ptr<DiscoInfo> discoInfo(storage->getDiscoInfo(capsInfo1->getVersion()));
            CPPUNIT_ASSERT(discoInfo);
            CPPUNIT_ASSERT(discoInfo->hasFeature("http://swift.im/feature1"));
        }

        void testReceiveIncorrectVerificationDiscoDoesNotStoreCaps() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendDiscoInfoResult(discoInfo2);

            std::shared_ptr<DiscoInfo> discoInfo(storage->getDiscoInfo(capsInfo1->getVersion()));
            CPPUNIT_ASSERT(!discoInfo);
        }

        void testReceiveFailingDiscoFallsBack() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendPresenceWithCaps(user2, capsInfo1alt);
            stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID()));

            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(1, user2, IQ::Get));
            std::shared_ptr<DiscoInfo> discoInfo(stanzaChannel->sentStanzas[1]->getPayload<DiscoInfo>());
            CPPUNIT_ASSERT(discoInfo);
            CPPUNIT_ASSERT_EQUAL("http://node2.im#" + capsInfo1alt->getVersion(), discoInfo->getNode());
        }

        void testReceiveNoDiscoFallsBack() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendPresenceWithCaps(user2, capsInfo1alt);
            stanzaChannel->onIQReceived(IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID(), std::shared_ptr<DiscoInfo>()));

            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(1, user2, IQ::Get));
            std::shared_ptr<DiscoInfo> discoInfo(stanzaChannel->sentStanzas[1]->getPayload<DiscoInfo>());
            CPPUNIT_ASSERT(discoInfo);
            CPPUNIT_ASSERT_EQUAL("http://node2.im#" + capsInfo1alt->getVersion(), discoInfo->getNode());
        }

        void testReceiveFailingFallbackDiscoFallsBack() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendPresenceWithCaps(user2, capsInfo1alt);
            sendPresenceWithCaps(user3, capsInfo1);
            stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID()));
            stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[1]->getTo(), stanzaChannel->sentStanzas[1]->getID()));

            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(2, user3, IQ::Get));
        }

        void testReceiveSameHashFromFailingUserAfterReconnectRequestsDisco() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID()));
            stanzaChannel->setAvailable(false);
            stanzaChannel->setAvailable(true);
            stanzaChannel->sentStanzas.clear();

            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user1, IQ::Get));
        }

        void testReconnectResetsFallback() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            sendPresenceWithCaps(user2, capsInfo1alt);
            stanzaChannel->setAvailable(false);
            stanzaChannel->setAvailable(true);
            stanzaChannel->sentStanzas.clear();
            sendPresenceWithCaps(user1, capsInfo1);
            stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID()));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
        }

        void testReconnectResetsRequests() {
            std::shared_ptr<CapsManager> testling = createManager();
            sendPresenceWithCaps(user1, capsInfo1);
            stanzaChannel->sentStanzas.clear();
            stanzaChannel->setAvailable(false);
            stanzaChannel->setAvailable(true);
            sendPresenceWithCaps(user1, capsInfo1);

            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user1, IQ::Get));
        }

    private:
        std::shared_ptr<CapsManager> createManager() {
            std::shared_ptr<CapsManager> manager(new CapsManager(storage.get(), stanzaChannel.get(), iqRouter.get(), crypto.get()));
            manager->setWarnOnInvalidHash(false);
            //manager->onCapsChanged.connect(boost::bind(&CapsManagerTest::handleCapsChanged, this, _1));
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

        void sendDiscoInfoResult(std::shared_ptr<DiscoInfo> discoInfo) {
            stanzaChannel->onIQReceived(IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID(), discoInfo));
        }

    private:
        std::unique_ptr<DummyStanzaChannel> stanzaChannel;
        std::unique_ptr<IQRouter> iqRouter;
        std::unique_ptr<CapsStorage> storage;
        std::vector<JID> changes;
        JID user1;
        std::shared_ptr<DiscoInfo> discoInfo1;
        std::shared_ptr<CapsInfo> capsInfo1;
        std::shared_ptr<CapsInfo> capsInfo1alt;
        JID user2;
        std::shared_ptr<DiscoInfo> discoInfo2;
        std::shared_ptr<CapsInfo> capsInfo2;
        std::shared_ptr<CapsInfo> legacyCapsInfo;
        JID user3;
        std::shared_ptr<CryptoProvider> crypto;
};

CPPUNIT_TEST_SUITE_REGISTRATION(CapsManagerTest);
