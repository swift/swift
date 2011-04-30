/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <SwifTools/Linkify.h>

using namespace Swift;

class LinkifyTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkifyTest);
		CPPUNIT_TEST(testLinkify_URLWithResource);
		CPPUNIT_TEST(testLinkify_HTTPSURLWithResource);
		CPPUNIT_TEST(testLinkify_URLWithEmptyResource);
		CPPUNIT_TEST(testLinkify_BareURL);
		CPPUNIT_TEST(testLinkify_URLSurroundedByWhitespace);
		CPPUNIT_TEST(testLinkify_MultipleURLs);
		CPPUNIT_TEST(testLinkify_CamelCase);
		CPPUNIT_TEST(testLinkify_HierarchicalResource);
		CPPUNIT_TEST(testLinkify_Anchor);
		CPPUNIT_TEST(testLinkify_Plus);
		CPPUNIT_TEST(testLinkify_Tilde);
		CPPUNIT_TEST(testLinkify_Equal);
		CPPUNIT_TEST(testLinkify_Authentication);
		CPPUNIT_TEST(testLinkify_At);
		CPPUNIT_TEST(testLinkify_Amps);
		CPPUNIT_TEST(testLinkify_UnicodeCharacter);
		CPPUNIT_TEST(testLinkify_NewLine);
		CPPUNIT_TEST(testLinkify_Tab);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testLinkify_URLWithResource() {
			std::string result = Linkify::linkify("http://swift.im/blog");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://swift.im/blog\">http://swift.im/blog</a>"),
					result);
		}

		void testLinkify_HTTPSURLWithResource() {
			std::string result = Linkify::linkify("https://swift.im/blog");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"https://swift.im/blog\">https://swift.im/blog</a>"),
					result);
		}

		void testLinkify_URLWithEmptyResource() {
			std::string result = Linkify::linkify("http://swift.im/");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://swift.im/\">http://swift.im/</a>"),
					result);
		}


		void testLinkify_BareURL() {
			std::string result = Linkify::linkify("http://swift.im");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://swift.im\">http://swift.im</a>"),
					result);
		}

		void testLinkify_URLSurroundedByWhitespace() {
			std::string result = Linkify::linkify("Foo http://swift.im/blog Bar");

			CPPUNIT_ASSERT_EQUAL(
					std::string("Foo <a href=\"http://swift.im/blog\">http://swift.im/blog</a> Bar"),
					result);
		}

		void testLinkify_MultipleURLs() {
			std::string result = Linkify::linkify("Foo http://swift.im/blog Bar http://el-tramo.be/about Baz");

			CPPUNIT_ASSERT_EQUAL(
					std::string("Foo <a href=\"http://swift.im/blog\">http://swift.im/blog</a> Bar <a href=\"http://el-tramo.be/about\">http://el-tramo.be/about</a> Baz"),
					result);
		}

		void testLinkify_CamelCase() {
			std::string result = Linkify::linkify("http://fOo.cOm/bAz");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://fOo.cOm/bAz\">http://fOo.cOm/bAz</a>"),
					result);
		}

		void testLinkify_HierarchicalResource() {
			std::string result = Linkify::linkify("http://foo.com/bar/baz/");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://foo.com/bar/baz/\">http://foo.com/bar/baz/</a>"),
					result);
		}

		void testLinkify_Anchor() {
			std::string result = Linkify::linkify("http://foo.com/bar#baz");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://foo.com/bar#baz\">http://foo.com/bar#baz</a>"),
					result);
		}

		void testLinkify_Plus() {
			std::string result = Linkify::linkify("http://foo.com/bar+baz");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://foo.com/bar+baz\">http://foo.com/bar+baz</a>"),
					result);
		}

		void testLinkify_Tilde() {
			std::string result = Linkify::linkify("http://foo.com/~kev/");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://foo.com/~kev/\">http://foo.com/~kev/</a>"),
					result);
		}

		void testLinkify_Equal() {
			std::string result = Linkify::linkify("http://www.amazon.co.uk/s/ref=nb_sb_noss?url=search-alias%3Daps&field-keywords=xmpp+definitive+guide&x=0&y=0");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://www.amazon.co.uk/s/ref=nb_sb_noss?url=search-alias%3Daps&field-keywords=xmpp+definitive+guide&x=0&y=0\">http://www.amazon.co.uk/s/ref=nb_sb_noss?url=search-alias%3Daps&field-keywords=xmpp+definitive+guide&x=0&y=0</a>"),
					result);
		}

		void testLinkify_Authentication() {
			std::string result = Linkify::linkify("http://bob:bla@swift.im/foo/bar");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://bob:bla@swift.im/foo/bar\">http://bob:bla@swift.im/foo/bar</a>"),
					result);
		}

		void testLinkify_At() {
			std::string result = Linkify::linkify("http://swift.im/foo@bar");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://swift.im/foo@bar\">http://swift.im/foo@bar</a>"),
					result);
		}

		void testLinkify_Amps() {
			std::string result = Linkify::linkify("http://swift.im/foo&bar&baz");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://swift.im/foo&bar&baz\">http://swift.im/foo&bar&baz</a>"),
					result);
		}

		void testLinkify_UnicodeCharacter() {
			std::string result = Linkify::linkify("http://\xe2\x98\x83.net");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://\xe2\x98\x83.net\">http://\xe2\x98\x83.net</a>"),
					result);
		}

		void testLinkify_NewLine() {
			std::string result = Linkify::linkify("http://swift.im\nfoo");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://swift.im\">http://swift.im</a>\nfoo"),
					result);
		}

		void testLinkify_Tab() {
			std::string result = Linkify::linkify("http://swift.im\tfoo");

			CPPUNIT_ASSERT_EQUAL(
					std::string("<a href=\"http://swift.im\">http://swift.im</a>\tfoo"),
					result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkifyTest);
