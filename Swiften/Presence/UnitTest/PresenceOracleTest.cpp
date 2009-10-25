#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Client/DummyStanzaChannel.h"

using namespace Swift;

class PresencePointerPair {
	public:
		boost::shared_ptr<Presence> one;
		boost::shared_ptr<Presence> two;
};

class PresenceOracleTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(PresenceOracleTest);
		CPPUNIT_TEST(testFirstPresence);
		CPPUNIT_TEST(testSecondPresence);
		CPPUNIT_TEST_SUITE_END();

	private:
		PresenceOracle* oracle_;
		DummyStanzaChannel* stanzaChannel_;

	public:

		void handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> lastPresence, PresencePointerPair* out) {
			CPPUNIT_ASSERT(out->one.get() == NULL);
			CPPUNIT_ASSERT(out->two.get() == NULL);
			out->one = newPresence;
			out->two = lastPresence;
			CPPUNIT_ASSERT(newPresence.get());
			CPPUNIT_ASSERT_EQUAL(newPresence, out->one);
		}
		
		void handlePresenceSubscriptionRequest(const JID& jid, const String& reason, boost::optional<JID>& outJID, boost::optional<String>& outReason) {
			CPPUNIT_ASSERT(!outJID);
			CPPUNIT_ASSERT(!outReason);
			outJID = jid;
			outReason = reason;
		}

		void setUp() {
			stanzaChannel_ = new DummyStanzaChannel();
			oracle_ = new PresenceOracle(stanzaChannel_);
		}

		void tearDown() {
			delete oracle_;
			delete stanzaChannel_;
		}

		void testFirstPresence() {
			PresencePointerPair out;
			oracle_->onPresenceChange.connect(boost::bind(&PresenceOracleTest::handlePresenceChange, this, _1, _2, &out));

			boost::optional<JID> jid;
			boost::optional<String> reason;
			oracle_->onPresenceSubscriptionRequest.connect(boost::bind(&PresenceOracleTest::handlePresenceSubscriptionRequest, this, _1, _2, jid, reason));

			boost::shared_ptr<Presence> sentPresence(new Presence("blarb"));
			stanzaChannel_->onPresenceReceived(sentPresence);

			CPPUNIT_ASSERT(!jid);
			CPPUNIT_ASSERT(!reason);
			CPPUNIT_ASSERT(out.two.get() == NULL);
			CPPUNIT_ASSERT_EQUAL(sentPresence, out.one);
		}
		
		void testSecondPresence() {
			PresencePointerPair out;
			oracle_->onPresenceChange.connect(boost::bind(&PresenceOracleTest::handlePresenceChange, this, _1, _2, &out));

			boost::shared_ptr<Presence> sentPresence1(new Presence("blarb"));
			stanzaChannel_->onPresenceReceived(sentPresence1);
			CPPUNIT_ASSERT_EQUAL(sentPresence1, out.one);
			out.one = boost::shared_ptr<Presence>();
			
			boost::optional<JID> jid;
			boost::optional<String> reason;
			oracle_->onPresenceSubscriptionRequest.connect(boost::bind(&PresenceOracleTest::handlePresenceSubscriptionRequest, this, _1, _2, jid, reason));

			boost::shared_ptr<Presence> sentPresence2(new Presence("test2"));
			stanzaChannel_->onPresenceReceived(sentPresence2);

			CPPUNIT_ASSERT(!jid);
			CPPUNIT_ASSERT(!reason);
			CPPUNIT_ASSERT_EQUAL(sentPresence1, out.two);
			CPPUNIT_ASSERT_EQUAL(sentPresence2, out.one);
		}

};
CPPUNIT_TEST_SUITE_REGISTRATION(PresenceOracleTest);

