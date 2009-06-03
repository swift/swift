#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Controllers/XMPPRosterController.h"
#include "Swiften/Elements/Payload.h"
#include "Swiften/Elements/RosterItemPayload.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Roster/XMPPRoster.h"

using namespace Swift;

class XMPPRosterControllerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(XMPPRosterControllerTest);
		CPPUNIT_TEST(testAdd);
		CPPUNIT_TEST(testModify);
		CPPUNIT_TEST(testRemove);
		CPPUNIT_TEST_SUITE_END();

		DummyIQChannel* channel_;
		IQRouter* router_;
	public:
		XMPPRosterControllerTest() {}

		void setUp() {
			channel_ = new DummyIQChannel();
			router_ = new IQRouter(channel_);
		}

		void tearDown() {
			delete channel_;
			delete router_;
		}

		void testAdd() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			XMPPRosterController controller(router_, xmppRoster);
			JID testling("foo@bar");
			CPPUNIT_ASSERT(!xmppRoster->containsJID(testling));
			boost::shared_ptr<Payload> payload(new RosterPayload());
			RosterItemPayload item(testling, "Bob", RosterItemPayload::Both);
			dynamic_cast<RosterPayload*>(payload.get())->addItem(item);
			controller.handleIQ(IQ::createRequest(IQ::Set, JID(), "eou", payload));
			CPPUNIT_ASSERT(xmppRoster->containsJID(testling));
		}

		void testModify() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			XMPPRosterController controller(router_, xmppRoster);
			JID testling("foo@bar");
			CPPUNIT_ASSERT(!xmppRoster->containsJID(testling));
			boost::shared_ptr<Payload> payload1(new RosterPayload());
			RosterItemPayload item1(testling, "Bob", RosterItemPayload::Both);
			dynamic_cast<RosterPayload*>(payload1.get())->addItem(item1);
			controller.handleIQ(IQ::createRequest(IQ::Set, JID(), "eou", payload1));
			CPPUNIT_ASSERT(xmppRoster->containsJID(testling));
			CPPUNIT_ASSERT_EQUAL(String("Bob"), xmppRoster->getNameForJID(testling));
			boost::shared_ptr<Payload> payload2(new RosterPayload());
			RosterItemPayload item2(testling, "Bob2", RosterItemPayload::Both);
			dynamic_cast<RosterPayload*>(payload2.get())->addItem(item2);
			controller.handleIQ(IQ::createRequest(IQ::Set, JID(), "eou", payload2));
			CPPUNIT_ASSERT_EQUAL(String("Bob2"), xmppRoster->getNameForJID(testling));
		}

		void testRemove() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			XMPPRosterController controller(router_, xmppRoster);
			JID testling("foo@bar");
			CPPUNIT_ASSERT(!xmppRoster->containsJID(testling));
			boost::shared_ptr<Payload> payload1(new RosterPayload());
			RosterItemPayload item1(testling, "Bob", RosterItemPayload::Both);
			dynamic_cast<RosterPayload*>(payload1.get())->addItem(item1);
			controller.handleIQ(IQ::createRequest(IQ::Set, JID(), "eou", payload1));
			CPPUNIT_ASSERT(xmppRoster->containsJID(testling));
			boost::shared_ptr<Payload> payload2(new RosterPayload());
			RosterItemPayload item2(testling, "Bob", RosterItemPayload::Remove);
			dynamic_cast<RosterPayload*>(payload2.get())->addItem(item2);
			controller.handleIQ(IQ::createRequest(IQ::Set, JID(), "eou", payload2));
			CPPUNIT_ASSERT(!xmppRoster->containsJID(testling));
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPRosterControllerTest);


