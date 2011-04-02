/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <SwifTools/URIHandler/XMPPURI.h>

using namespace Swift;

class XMPPURITest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(XMPPURITest);
		CPPUNIT_TEST(testFromString_Authority);
		CPPUNIT_TEST(testFromString_AuthorityWithPath);
		CPPUNIT_TEST(testFromString_AuthorityWithFragment);
		CPPUNIT_TEST(testFromString_AuthorityWithPathAndFragment);
		CPPUNIT_TEST(testFromString_AuthorityWithIntlChars);
		CPPUNIT_TEST(testFromString_AuthorityWithQueryWithoutParameters);
		CPPUNIT_TEST(testFromString_AuthorityWithQueryWithParameters);
		CPPUNIT_TEST(testFromString_AuthorityWithQueryWithoutParametersWithFragment);
		CPPUNIT_TEST(testFromString_AuthorityWithQueryWithParametersWithFragment);
		CPPUNIT_TEST(testFromString_Path);
		CPPUNIT_TEST(testFromString_PathWithFragment);
		CPPUNIT_TEST(testFromString_PathWithIntlChars);
		CPPUNIT_TEST(testFromString_PathWithInvalidEscapedChar);
		CPPUNIT_TEST(testFromString_PathWithIncompleteEscapedChar);
		CPPUNIT_TEST(testFromString_PathWithIncompleteEscapedChar2);
		CPPUNIT_TEST(testFromString_PathWithQueryWithoutParameters);
		CPPUNIT_TEST(testFromString_PathWithQueryWithParameters);
		CPPUNIT_TEST(testFromString_PathWithQueryWithoutParametersWithFragment);
		CPPUNIT_TEST(testFromString_PathWithQueryWithParametersWithFragment);
		CPPUNIT_TEST(testFromString_NoPrefix);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testFromString_Authority() {
			XMPPURI testling = XMPPURI::fromString("xmpp://foo@bar.com");

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), testling.getAuthority());
		}

		void testFromString_AuthorityWithPath() {
			XMPPURI testling = XMPPURI::fromString("xmpp://foo@bar.com/baz@example.com");

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), testling.getAuthority());
			CPPUNIT_ASSERT_EQUAL(JID("baz@example.com"), testling.getPath());
		}

		void testFromString_AuthorityWithFragment() {
			XMPPURI testling = XMPPURI::fromString("xmpp://foo@bar.com#myfragment");

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), testling.getAuthority());
			CPPUNIT_ASSERT_EQUAL(std::string("myfragment"), testling.getFragment());
		}

		void testFromString_AuthorityWithPathAndFragment() {
			XMPPURI testling = XMPPURI::fromString("xmpp://foo@bar.com/baz@example.com#myfragment");

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), testling.getAuthority());
			CPPUNIT_ASSERT_EQUAL(JID("baz@example.com"), testling.getPath());
			CPPUNIT_ASSERT_EQUAL(std::string("myfragment"), testling.getFragment());
		}

		void testFromString_AuthorityWithIntlChars() {
			XMPPURI testling = XMPPURI::fromString("xmpp://nasty!%23$%25()*+,-.;=\%3F\%5B\%5C\%5D\%5E_\%60\%7B\%7C\%7D~node@example.com");

			CPPUNIT_ASSERT_EQUAL(JID("nasty!#$\%()*+,-.;=?[\\]^_`{|}~node@example.com"), testling.getAuthority());
		}

		void testFromString_AuthorityWithQueryWithoutParameters() {
			XMPPURI testling = XMPPURI::fromString("xmpp://test@example.com?message");

			CPPUNIT_ASSERT_EQUAL(JID("test@example.com"), testling.getAuthority());
			CPPUNIT_ASSERT_EQUAL(std::string("message"), testling.getQueryType());
		}

		void testFromString_AuthorityWithQueryWithParameters() {
			XMPPURI testling = XMPPURI::fromString("xmpp://test@example.com?message;subject=Test%20Message;body=Here%27s%20a%20test%20message");

			CPPUNIT_ASSERT_EQUAL(JID("test@example.com"), testling.getAuthority());
			CPPUNIT_ASSERT_EQUAL(std::string("message"), testling.getQueryType());
			CPPUNIT_ASSERT_EQUAL(std::string("Test Message"), get(testling.getQueryParameters(), "subject"));
			CPPUNIT_ASSERT_EQUAL(std::string("Here's a test message"), get(testling.getQueryParameters(), "body"));
		}

		void testFromString_AuthorityWithQueryWithoutParametersWithFragment() {
			XMPPURI testling = XMPPURI::fromString("xmpp://test@example.com?message#myfragment");

			CPPUNIT_ASSERT_EQUAL(JID("test@example.com"), testling.getAuthority());
			CPPUNIT_ASSERT_EQUAL(std::string("message"), testling.getQueryType());
			CPPUNIT_ASSERT_EQUAL(std::string("myfragment"), testling.getFragment());
		}

		void testFromString_AuthorityWithQueryWithParametersWithFragment() {
			XMPPURI testling = XMPPURI::fromString("xmpp://test@example.com?message;subject=Test%20Message;body=Here%27s%20a%20test%20message#myfragment");

			CPPUNIT_ASSERT_EQUAL(JID("test@example.com"), testling.getAuthority());
			CPPUNIT_ASSERT_EQUAL(std::string("message"), testling.getQueryType());
			CPPUNIT_ASSERT_EQUAL(std::string("Test Message"), get(testling.getQueryParameters(), "subject"));
			CPPUNIT_ASSERT_EQUAL(std::string("Here's a test message"), get(testling.getQueryParameters(), "body"));
			CPPUNIT_ASSERT_EQUAL(std::string("myfragment"), testling.getFragment());
		}

		void testFromString_Path() {
			XMPPURI testling = XMPPURI::fromString("xmpp:baz@example.com");

			CPPUNIT_ASSERT_EQUAL(JID("baz@example.com"), testling.getPath());
		}

		void testFromString_PathWithFragment() {
			XMPPURI testling = XMPPURI::fromString("xmpp:baz@example.com#myfragment");

			CPPUNIT_ASSERT_EQUAL(JID("baz@example.com"), testling.getPath());
			CPPUNIT_ASSERT_EQUAL(std::string("myfragment"), testling.getFragment());
		}

		void testFromString_PathWithIntlChars() {
			XMPPURI testling = XMPPURI::fromString("xmpp:nasty!%23$%25()*+,-.;=\%3F\%5B\%5C\%5D\%5E_\%60\%7B\%7C\%7D~node@example.com");

			CPPUNIT_ASSERT_EQUAL(JID("nasty!#$\%()*+,-.;=?[\\]^_`{|}~node@example.com"), testling.getPath());
		}

		void testFromString_PathWithInvalidEscapedChar() {
			XMPPURI testling = XMPPURI::fromString("xmpp:test%%@example.com");

			CPPUNIT_ASSERT_EQUAL(JID(), testling.getPath());
		}

		void testFromString_PathWithIncompleteEscapedChar() {
			XMPPURI testling = XMPPURI::fromString("xmpp:test@example.com%");

			CPPUNIT_ASSERT_EQUAL(JID(), testling.getPath());
		}

		void testFromString_PathWithIncompleteEscapedChar2() {
			XMPPURI testling = XMPPURI::fromString("xmpp:test@example.com%1");

			CPPUNIT_ASSERT_EQUAL(JID(), testling.getPath());
		}

		void testFromString_PathWithQueryWithoutParameters() {
			XMPPURI testling = XMPPURI::fromString("xmpp:test@example.com?message");

			CPPUNIT_ASSERT_EQUAL(JID("test@example.com"), testling.getPath());
			CPPUNIT_ASSERT_EQUAL(std::string("message"), testling.getQueryType());
		}

		void testFromString_PathWithQueryWithParameters() {
			XMPPURI testling = XMPPURI::fromString("xmpp:test@example.com?message;subject=Test%20Message;body=Here%27s%20a%20test%20message");

			CPPUNIT_ASSERT_EQUAL(JID("test@example.com"), testling.getPath());
			CPPUNIT_ASSERT_EQUAL(std::string("message"), testling.getQueryType());
			CPPUNIT_ASSERT_EQUAL(std::string("Test Message"), get(testling.getQueryParameters(), "subject"));
			CPPUNIT_ASSERT_EQUAL(std::string("Here's a test message"), get(testling.getQueryParameters(), "body"));
		}

		void testFromString_PathWithQueryWithoutParametersWithFragment() {
			XMPPURI testling = XMPPURI::fromString("xmpp:test@example.com?message#myfragment");

			CPPUNIT_ASSERT_EQUAL(JID("test@example.com"), testling.getPath());
			CPPUNIT_ASSERT_EQUAL(std::string("message"), testling.getQueryType());
			CPPUNIT_ASSERT_EQUAL(std::string("myfragment"), testling.getFragment());
		}

		void testFromString_PathWithQueryWithParametersWithFragment() {
			XMPPURI testling = XMPPURI::fromString("xmpp:test@example.com?message;subject=Test%20Message;body=Here%27s%20a%20test%20message#myfragment");

			CPPUNIT_ASSERT_EQUAL(JID("test@example.com"), testling.getPath());
			CPPUNIT_ASSERT_EQUAL(std::string("message"), testling.getQueryType());
			CPPUNIT_ASSERT_EQUAL(std::string("Test Message"), get(testling.getQueryParameters(), "subject"));
			CPPUNIT_ASSERT_EQUAL(std::string("Here's a test message"), get(testling.getQueryParameters(), "body"));
			CPPUNIT_ASSERT_EQUAL(std::string("myfragment"), testling.getFragment());
		}

		void testFromString_NoPrefix() {
			XMPPURI testling = XMPPURI::fromString("baz@example.com");

			CPPUNIT_ASSERT(testling.isNull());
		}

	private:
		std::string get(const std::map<std::string, std::string>& m, const std::string& k) {
			std::map<std::string, std::string>::const_iterator i = m.find(k);
			return i == m.end() ? "" : i->second;
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPURITest);
