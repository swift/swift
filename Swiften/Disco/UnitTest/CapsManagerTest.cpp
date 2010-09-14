/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>
#include <boost/bind.hpp>

#include "Swiften/Disco/CapsManager.h"
#include "Swiften/Disco/CapsMemoryStorage.h"
#include "Swiften/Disco/CapsInfoGenerator.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Elements/CapsInfo.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Client/DummyStanzaChannel.h"

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
		CPPUNIT_TEST(testReceiveFailingFallbackDiscoFallsBack);
		CPPUNIT_TEST(testReceiveSameHashFromFailingUserAfterReconnectRequestsDisco);
		CPPUNIT_TEST(testReconnectResetsFallback);
		CPPUNIT_TEST(testReconnectResetsRequests);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			stanzaChannel = new DummyStanzaChannel();
			iqRouter = new IQRouter(stanzaChannel);
			storage = new CapsMemoryStorage();
			user1 = JID("user1@bar.com/bla");
			discoInfo1 = boost::shared_ptr<DiscoInfo>(new DiscoInfo());
			discoInfo1->addFeature("http://swift.im/feature1");
			capsInfo1 = boost::shared_ptr<CapsInfo>(new CapsInfo(CapsInfoGenerator("http://node1.im").generateCapsInfo(*discoInfo1.get())));
			capsInfo1alt = boost::shared_ptr<CapsInfo>(new CapsInfo(CapsInfoGenerator("http://node2.im").generateCapsInfo(*discoInfo1.get())));
			user2 = JID("user2@foo.com/baz");
			discoInfo2 = boost::shared_ptr<DiscoInfo>(new DiscoInfo());
			discoInfo2->addFeature("http://swift.im/feature2");
			capsInfo2 = boost::shared_ptr<CapsInfo>(new CapsInfo(CapsInfoGenerator("http://node2.im").generateCapsInfo(*discoInfo2.get())));
			user3 = JID("user3@foo.com/baz");
			legacyCapsInfo = boost::shared_ptr<CapsInfo>(new CapsInfo("http://swift.im", "ver1", ""));
		}

		void tearDown() {
			delete storage;
			delete iqRouter;
			delete stanzaChannel;
		}

		void testReceiveNewHashRequestsDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user1, IQ::Get));
			boost::shared_ptr<DiscoInfo> discoInfo(stanzaChannel->sentStanzas[0]->getPayload<DiscoInfo>());
			CPPUNIT_ASSERT(discoInfo);
			CPPUNIT_ASSERT_EQUAL("http://node1.im#" + capsInfo1->getVersion(), discoInfo->getNode());
		}

		void testReceiveSameHashDoesNotRequestDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			stanzaChannel->sentStanzas.clear();
			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testReceiveLegacyCapsDoesNotRequestDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, legacyCapsInfo);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testReceiveSameHashAfterSuccesfulDiscoDoesNotRequestDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			sendDiscoInfoResult(discoInfo1);

			stanzaChannel->sentStanzas.clear();
			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testReceiveSameHashFromSameUserAfterFailedDiscoDoesNotRequestDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

			stanzaChannel->sentStanzas.clear();
			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testReceiveSameHashFromSameUserAfterIncorrectVerificationDoesNotRequestDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			sendDiscoInfoResult(discoInfo2);

			stanzaChannel->sentStanzas.clear();
			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testReceiveSameHashFromDifferentUserAfterFailedDiscoRequestsDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

			stanzaChannel->sentStanzas.clear();
			sendPresenceWithCaps(user2, capsInfo1);
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user2, IQ::Get));
		}

		void testReceiveSameHashFromDifferentUserAfterIncorrectVerificationRequestsDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			sendDiscoInfoResult(discoInfo2);

			stanzaChannel->sentStanzas.clear();
			sendPresenceWithCaps(user2, capsInfo1);
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user2, IQ::Get));
		}

		void testReceiveDifferentHashFromSameUserAfterFailedDiscoDoesNotRequestDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

			stanzaChannel->sentStanzas.clear();
			sendPresenceWithCaps(user1, capsInfo2);

			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user1, IQ::Get));
		}

		void testReceiveSuccesfulDiscoStoresCaps() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			sendDiscoInfoResult(discoInfo1);

			boost::shared_ptr<DiscoInfo> discoInfo(storage->getDiscoInfo(capsInfo1->getVersion()));
			CPPUNIT_ASSERT(discoInfo);
			CPPUNIT_ASSERT(discoInfo->hasFeature("http://swift.im/feature1"));
		}

		void testReceiveIncorrectVerificationDiscoDoesNotStoreCaps() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			sendDiscoInfoResult(discoInfo2);

			boost::shared_ptr<DiscoInfo> discoInfo(storage->getDiscoInfo(capsInfo1->getVersion()));
			CPPUNIT_ASSERT(!discoInfo);
		}

		void testReceiveFailingDiscoFallsBack() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			sendPresenceWithCaps(user2, capsInfo1alt);
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(1, user2, IQ::Get));
			boost::shared_ptr<DiscoInfo> discoInfo(stanzaChannel->sentStanzas[1]->getPayload<DiscoInfo>());
			CPPUNIT_ASSERT(discoInfo);
			CPPUNIT_ASSERT_EQUAL("http://node2.im#" + capsInfo1alt->getVersion(), discoInfo->getNode());
		}

		void testReceiveFailingFallbackDiscoFallsBack() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			sendPresenceWithCaps(user2, capsInfo1alt);
			sendPresenceWithCaps(user3, capsInfo1);
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[1]->getID()));

			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(2, user3, IQ::Get));
		}

		void testReceiveSameHashFromFailingUserAfterReconnectRequestsDisco() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));
			stanzaChannel->setAvailable(false);
			stanzaChannel->setAvailable(true);
			stanzaChannel->sentStanzas.clear();

			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user1, IQ::Get));
		}

		void testReconnectResetsFallback() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			sendPresenceWithCaps(user2, capsInfo1alt);
			stanzaChannel->setAvailable(false);
			stanzaChannel->setAvailable(true);
			stanzaChannel->sentStanzas.clear();
			sendPresenceWithCaps(user1, capsInfo1);
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testReconnectResetsRequests() {
			std::auto_ptr<CapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);
			stanzaChannel->sentStanzas.clear();
			stanzaChannel->setAvailable(false);
			stanzaChannel->setAvailable(true);
			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<DiscoInfo>(0, user1, IQ::Get));
		}

	private:
		std::auto_ptr<CapsManager> createManager() {
			std::auto_ptr<CapsManager> manager(new CapsManager(storage, stanzaChannel, iqRouter));
			manager->setWarnOnInvalidHash(false);
			//manager->onCapsChanged.connect(boost::bind(&CapsManagerTest::handleCapsChanged, this, _1));
			return manager;
		}

		void handleCapsChanged(const JID& jid) {
			changes.push_back(jid);
		}

		void sendPresenceWithCaps(const JID& jid, boost::shared_ptr<CapsInfo> caps) {
			boost::shared_ptr<Presence> presence(new Presence());
			presence->setFrom(jid);
			presence->addPayload(caps);
			stanzaChannel->onPresenceReceived(presence);
		}

		void sendDiscoInfoResult(boost::shared_ptr<DiscoInfo> discoInfo) {
			stanzaChannel->onIQReceived(IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[0]->getID(), discoInfo));
		}

	private:
		DummyStanzaChannel* stanzaChannel;
		IQRouter* iqRouter;
		CapsStorage* storage;
		std::vector<JID> changes;
		JID user1;
		boost::shared_ptr<DiscoInfo> discoInfo1;
		boost::shared_ptr<CapsInfo> capsInfo1;
		boost::shared_ptr<CapsInfo> capsInfo1alt;
		JID user2;
		boost::shared_ptr<DiscoInfo> discoInfo2;
		boost::shared_ptr<CapsInfo> capsInfo2;
		boost::shared_ptr<CapsInfo> legacyCapsInfo;
		JID user3;
};

CPPUNIT_TEST_SUITE_REGISTRATION(CapsManagerTest);
