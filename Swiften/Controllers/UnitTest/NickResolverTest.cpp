#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Controllers/NickResolver.h"
#include "Swiften/Roster/XMPPRoster.h"

using namespace Swift;

class NickResolverTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(NickResolverTest);
		CPPUNIT_TEST(testNoMatch);
		CPPUNIT_TEST(testMatch);
		CPPUNIT_TEST(testOverwrittenMatch);
		CPPUNIT_TEST(testRemovedMatch);
		CPPUNIT_TEST_SUITE_END();

		std::vector<String> groups_;

	public:
		NickResolverTest() {}

		void testNoMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testling("foo@bar/baz");

			CPPUNIT_ASSERT_EQUAL(String("foo@bar"), resolver.jidToNick(testling));
		}

		void testMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testling("foo@bar/baz");
			xmppRoster->addContact(testling, "Test", groups_);

			CPPUNIT_ASSERT_EQUAL(String("Test"), resolver.jidToNick(testling));
		}

		void testOverwrittenMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testling("foo@bar/baz");
			xmppRoster->addContact(testling, "FailTest", groups_);
			xmppRoster->addContact(testling, "Test", groups_);

			CPPUNIT_ASSERT_EQUAL(String("Test"), resolver.jidToNick(testling));
		}

		void testRemovedMatch() {
			boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());
			NickResolver resolver(xmppRoster);
			JID testling("foo@bar/baz");
			xmppRoster->addContact(testling, "FailTest", groups_);
			xmppRoster->removeContact(testling);
			CPPUNIT_ASSERT_EQUAL(String("foo@bar"), resolver.jidToNick(testling));
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION(NickResolverTest);

