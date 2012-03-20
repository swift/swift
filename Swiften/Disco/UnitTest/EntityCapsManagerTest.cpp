/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>
#include <boost/bind.hpp>

#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/Disco/CapsProvider.h>
#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Disco/CapsInfoGenerator.h>

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
			stanzaChannel = new DummyStanzaChannel();
			capsProvider = new DummyCapsProvider();

			user1 = JID("user1@bar.com/bla");
			discoInfo1 = boost::make_shared<DiscoInfo>();
			discoInfo1->addFeature("http://swift.im/feature1");
			capsInfo1 = boost::make_shared<CapsInfo>(CapsInfoGenerator("http://node1.im").generateCapsInfo(*discoInfo1.get()));
			capsInfo1alt = boost::make_shared<CapsInfo>(CapsInfoGenerator("http://node2.im").generateCapsInfo(*discoInfo1.get()));
			user2 = JID("user2@foo.com/baz");
			discoInfo2 = boost::make_shared<DiscoInfo>();
			discoInfo2->addFeature("http://swift.im/feature2");
			capsInfo2 = boost::make_shared<CapsInfo>(CapsInfoGenerator("http://node2.im").generateCapsInfo(*discoInfo2.get()));
			user3 = JID("user3@foo.com/baz");
			legacyCapsInfo = boost::make_shared<CapsInfo>("http://swift.im", "ver1", "");
		}

		void tearDown() {
			delete capsProvider;
			delete stanzaChannel;
		}

		void testReceiveKnownHash() {
			boost::shared_ptr<EntityCapsManager> testling = createManager();
			capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
			CPPUNIT_ASSERT_EQUAL(discoInfo1, testling->getCaps(user1));
		}

		void testReceiveKnownHashTwiceDoesNotTriggerChange() {
			boost::shared_ptr<EntityCapsManager> testling = createManager();
			capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
			sendPresenceWithCaps(user1, capsInfo1);
			changes.clear();

			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
		}

		void testReceiveUnknownHashDoesNotTriggerChange() {
			boost::shared_ptr<EntityCapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
		}

		void testHashAvailable() {
			boost::shared_ptr<EntityCapsManager> testling = createManager();
			sendPresenceWithCaps(user1, capsInfo1);

			capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
			capsProvider->onCapsAvailable(capsInfo1->getVersion());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
			CPPUNIT_ASSERT_EQUAL(discoInfo1, testling->getCaps(user1));
		}

		void testReceiveUnknownHashAfterKnownHashTriggersChangeAndClearsCaps() {
			boost::shared_ptr<EntityCapsManager> testling = createManager();
			capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
			sendPresenceWithCaps(user1, capsInfo1);
			changes.clear();
			sendPresenceWithCaps(user1, capsInfo2);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
			CPPUNIT_ASSERT(!testling->getCaps(user1));
		}

		void testReceiveUnavailablePresenceAfterKnownHashTriggersChangeAndClearsCaps() {
			boost::shared_ptr<EntityCapsManager> testling = createManager();
			capsProvider->caps[capsInfo1->getVersion()] = discoInfo1;
			sendPresenceWithCaps(user1, capsInfo1);
			changes.clear();
			sendUnavailablePresence(user1);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
			CPPUNIT_ASSERT(!testling->getCaps(user1));
		}

		void testReconnectTriggersChangeAndClearsCaps() {
			boost::shared_ptr<EntityCapsManager> testling = createManager();
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
		boost::shared_ptr<EntityCapsManager> createManager() {
			boost::shared_ptr<EntityCapsManager> manager(new EntityCapsManager(capsProvider, stanzaChannel));
			manager->onCapsChanged.connect(boost::bind(&EntityCapsManagerTest::handleCapsChanged, this, _1));
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

		void sendUnavailablePresence(const JID& jid) {
			boost::shared_ptr<Presence> presence(new Presence());
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
		DummyStanzaChannel* stanzaChannel;
		DummyCapsProvider* capsProvider;
		JID user1;
		boost::shared_ptr<DiscoInfo> discoInfo1;
		boost::shared_ptr<CapsInfo> capsInfo1;
		boost::shared_ptr<CapsInfo> capsInfo1alt;
		JID user2;
		boost::shared_ptr<DiscoInfo> discoInfo2;
		boost::shared_ptr<CapsInfo> capsInfo2;
		boost::shared_ptr<CapsInfo> legacyCapsInfo;
		JID user3;
		std::vector<JID> changes;
};

CPPUNIT_TEST_SUITE_REGISTRATION(EntityCapsManagerTest);
