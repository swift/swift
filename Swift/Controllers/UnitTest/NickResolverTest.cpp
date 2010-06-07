/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/UnitTest/MockMUCRegistry.h"
#include "Swiften/Roster/XMPPRoster.h"

using namespace Swift;

class NickResolverTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(NickResolverTest);
		CPPUNIT_TEST(testNoMatch);
		CPPUNIT_TEST(testZeroLengthMatch);
		CPPUNIT_TEST(testMatch);
		CPPUNIT_TEST(testOverwrittenMatch);
		CPPUNIT_TEST(testRemovedMatch);
		CPPUNIT_TEST(testMUCNick);
		CPPUNIT_TEST(testMUCNoNick);
		CPPUNIT_TEST(testRemovedMatch);
		CPPUNIT_TEST_SUITE_END();

		std::vector<String> groups_;

	public:
		NickResolverTest() {}

		void testMUCNick() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			MockMUCRegistry registry;
			resolver.setMUCRegistry(&registry);
			registry.setNext(true);
			JID testJID("foo@bar/baz");

			CPPUNIT_ASSERT_EQUAL(String("baz"), resolver.jidToNick(testJID));
		}

		void testMUCNoNick() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			MockMUCRegistry registry;
			resolver.setMUCRegistry(&registry);
			registry.setNext(true);
			JID testJID("foo@bar");

			CPPUNIT_ASSERT_EQUAL(String("foo@bar"), resolver.jidToNick(testJID));
		}


		void testNoMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testJID("foo@bar/baz");

			CPPUNIT_ASSERT_EQUAL(String("foo@bar"), resolver.jidToNick(testJID));
		}
		
		void testZeroLengthMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testJID("foo@bar/baz");
			xmppRoster->addContact(testJID, "", groups_, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(String("foo@bar"), resolver.jidToNick(testJID));
		}

		void testMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testJID("foo@bar/baz");
			xmppRoster->addContact(testJID, "Test", groups_, RosterItemPayload::Both);

			CPPUNIT_ASSERT_EQUAL(String("Test"), resolver.jidToNick(testJID));
		}

		void testOverwrittenMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testJID("foo@bar/baz");
			xmppRoster->addContact(testJID, "FailTest", groups_, RosterItemPayload::Both);
			xmppRoster->addContact(testJID, "Test", groups_, RosterItemPayload::Both);

			CPPUNIT_ASSERT_EQUAL(String("Test"), resolver.jidToNick(testJID));
		}

		void testRemovedMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testJID("foo@bar/baz");
			xmppRoster->addContact(testJID, "FailTest", groups_, RosterItemPayload::Both);
			xmppRoster->removeContact(testJID);
			CPPUNIT_ASSERT_EQUAL(String("foo@bar"), resolver.jidToNick(testJID));
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION(NickResolverTest);

