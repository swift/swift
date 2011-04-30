/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Presence/SubscriptionManager.h>

using namespace Swift;

class PresenceOracleTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PresenceOracleTest);
		CPPUNIT_TEST(testReceivePresence);
		CPPUNIT_TEST(testReceivePresenceFromDifferentResources);
		CPPUNIT_TEST(testSubscriptionRequest);
		CPPUNIT_TEST(testReconnectResetsPresences);
		CPPUNIT_TEST(testHighestPresenceSingle);
		CPPUNIT_TEST(testHighestPresenceMultiple);
		CPPUNIT_TEST(testHighestPresenceGlobal);
		CPPUNIT_TEST(testHighestPresenceChangePriority);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			stanzaChannel_ = new DummyStanzaChannel();
			oracle_ = new PresenceOracle(stanzaChannel_);
			oracle_->onPresenceChange.connect(boost::bind(&PresenceOracleTest::handlePresenceChange, this, _1));
			subscriptionManager_ = new SubscriptionManager(stanzaChannel_);
			subscriptionManager_->onPresenceSubscriptionRequest.connect(boost::bind(&PresenceOracleTest::handlePresenceSubscriptionRequest, this, _1, _2));
			user1 = JID("user1@foo.com/Foo");
			user1alt = JID("user1@foo.com/Bar");
			user2 = JID("user2@bar.com/Bar");
		}

		void tearDown() {
			delete subscriptionManager_;
			delete oracle_;
			delete stanzaChannel_;
		}

		void testHighestPresenceSingle() {
					JID bareJID("alice@wonderland.lit");
					Presence::ref fiveOn = makeOnline("blah", 5);
					Presence::ref fiveOff = makeOffline("/blah");
					CPPUNIT_ASSERT_EQUAL(Presence::ref(), oracle_->getHighestPriorityPresence(bareJID));
					stanzaChannel_->onPresenceReceived(fiveOn);
					CPPUNIT_ASSERT_EQUAL(fiveOn, oracle_->getHighestPriorityPresence(bareJID));
					stanzaChannel_->onPresenceReceived(fiveOff);
					CPPUNIT_ASSERT_EQUAL(fiveOff, oracle_->getHighestPriorityPresence(bareJID));
				}

		void testHighestPresenceMultiple() {
			JID bareJID("alice@wonderland.lit");
			Presence::ref fiveOn = makeOnline("blah", 5);
			Presence::ref fiveOff = makeOffline("/blah");
			Presence::ref tenOn = makeOnline("bert", 10);
			Presence::ref tenOff = makeOffline("/bert");
			stanzaChannel_->onPresenceReceived(fiveOn);
			stanzaChannel_->onPresenceReceived(tenOn);
			CPPUNIT_ASSERT_EQUAL(tenOn, oracle_->getHighestPriorityPresence(bareJID));
			stanzaChannel_->onPresenceReceived(fiveOff);
			CPPUNIT_ASSERT_EQUAL(tenOn, oracle_->getHighestPriorityPresence(bareJID));
			stanzaChannel_->onPresenceReceived(fiveOn);
			stanzaChannel_->onPresenceReceived(tenOff);
			CPPUNIT_ASSERT_EQUAL(fiveOn, oracle_->getHighestPriorityPresence(bareJID));
		}

		void testHighestPresenceGlobal() {
			JID bareJID("alice@wonderland.lit");
			Presence::ref fiveOn = makeOnline("blah", 5);
			Presence::ref fiveOff = makeOffline("/blah");
			Presence::ref tenOn = makeOnline("bert", 10);
			Presence::ref allOff = makeOffline("");
			stanzaChannel_->onPresenceReceived(fiveOn);
			stanzaChannel_->onPresenceReceived(tenOn);
			stanzaChannel_->onPresenceReceived(allOff);
			CPPUNIT_ASSERT_EQUAL(allOff, oracle_->getHighestPriorityPresence(bareJID));
		}

		void testHighestPresenceChangePriority() {
			JID bareJID("alice@wonderland.lit");
			Presence::ref fiveOn = makeOnline("blah", 5);
			Presence::ref fiveOff = makeOffline("/blah");
			Presence::ref tenOn = makeOnline("bert", 10);
			Presence::ref tenOnThree = makeOnline("bert", 3);
			Presence::ref tenOff = makeOffline("/bert");
			stanzaChannel_->onPresenceReceived(fiveOn);
			stanzaChannel_->onPresenceReceived(tenOn);
			stanzaChannel_->onPresenceReceived(tenOnThree);
			CPPUNIT_ASSERT_EQUAL(fiveOn, oracle_->getHighestPriorityPresence(bareJID));
			stanzaChannel_->onPresenceReceived(fiveOff);
			CPPUNIT_ASSERT_EQUAL(tenOnThree, oracle_->getHighestPriorityPresence(bareJID));
			stanzaChannel_->onPresenceReceived(fiveOn);
			CPPUNIT_ASSERT_EQUAL(fiveOn, oracle_->getHighestPriorityPresence(bareJID));
		}

		void testReceivePresence() {
			boost::shared_ptr<Presence> sentPresence(createPresence(user1));
			stanzaChannel_->onPresenceReceived(sentPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(subscriptionRequests.size()));
			CPPUNIT_ASSERT_EQUAL(sentPresence, changes[0]);
			CPPUNIT_ASSERT_EQUAL(sentPresence, oracle_->getLastPresence(user1));
		}

		void testReceivePresenceFromDifferentResources() {
			boost::shared_ptr<Presence> sentPresence1(createPresence(user1));
			boost::shared_ptr<Presence> sentPresence2(createPresence(user1alt));
			stanzaChannel_->onPresenceReceived(sentPresence1);
			stanzaChannel_->onPresenceReceived(sentPresence2);

			CPPUNIT_ASSERT_EQUAL(sentPresence1, oracle_->getLastPresence(user1));
			CPPUNIT_ASSERT_EQUAL(sentPresence2, oracle_->getLastPresence(user1alt));
		}
		
		void testSubscriptionRequest() {
			std::string reasonText = "Because I want to";
			JID sentJID = JID("me@example.com");

			boost::shared_ptr<Presence> sentPresence(new Presence());
			sentPresence->setType(Presence::Subscribe);
			sentPresence->setFrom(sentJID);
			sentPresence->setStatus(reasonText);
			stanzaChannel_->onPresenceReceived(sentPresence);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(subscriptionRequests.size()));
			CPPUNIT_ASSERT_EQUAL(sentJID, subscriptionRequests[0].jid);
			CPPUNIT_ASSERT_EQUAL(reasonText, subscriptionRequests[0].reason);
		}

		void testReconnectResetsPresences() {
			boost::shared_ptr<Presence> sentPresence(createPresence(user1));
			stanzaChannel_->onPresenceReceived(sentPresence);
			stanzaChannel_->setAvailable(false);
			stanzaChannel_->setAvailable(true);

			CPPUNIT_ASSERT(!oracle_->getLastPresence(user1));
		}
	
	private:
		Presence::ref makeOnline(const std::string& resource, int priority) {
			Presence::ref presence(new Presence());
			presence->setPriority(priority);
			presence->setFrom(JID("alice@wonderland.lit/" + resource));
			return presence;
		}

		Presence::ref makeOffline(const std::string& resource) {
			Presence::ref presence(new Presence());
			presence->setFrom(JID("alice@wonderland.lit" + resource));
			presence->setType(Presence::Unavailable);
			return presence;
		}

		void handlePresenceChange(boost::shared_ptr<Presence> newPresence) {
			changes.push_back(newPresence);
		}
		
		void handlePresenceSubscriptionRequest(const JID& jid, const std::string& reason) {
			SubscriptionRequestInfo subscriptionRequest;
			subscriptionRequest.jid = jid;
			subscriptionRequest.reason = reason;
			subscriptionRequests.push_back(subscriptionRequest);
		}

		boost::shared_ptr<Presence> createPresence(const JID& jid) {
			boost::shared_ptr<Presence> sentPresence(new Presence("blarb"));
			sentPresence->setFrom(jid);
			return sentPresence;
		}

	private:
		struct SubscriptionRequestInfo {
			JID jid;
			std::string reason;
		};
		PresenceOracle* oracle_;
		SubscriptionManager* subscriptionManager_;
		DummyStanzaChannel* stanzaChannel_;
		std::vector<Presence::ref> changes;
		std::vector<SubscriptionRequestInfo> subscriptionRequests;
		JID user1;
		JID user1alt;
		JID user2;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PresenceOracleTest);

