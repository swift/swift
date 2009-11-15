#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "SwifTools/Linkify.h"

using namespace Swift;

class LinkifyTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkifyTest);
		CPPUNIT_TEST(testLinkify_URLWithResource);
		CPPUNIT_TEST(testLinkify_URLWithEmptyResource);
		CPPUNIT_TEST(testLinkify_BareURL);
		CPPUNIT_TEST(testLinkify_URLSurroundedByWhitespace);
		CPPUNIT_TEST(testLinkify_MultipleURLs);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testLinkify_URLWithResource() {
			String result = Linkify::linkify("http://swift.im/blog");

			CPPUNIT_ASSERT_EQUAL(
					String("<a href=\"http://swift.im/blog\">http://swift.im/blog</a>"),
					result);
		}

		void testLinkify_URLWithEmptyResource() {
			String result = Linkify::linkify("http://swift.im/");

			CPPUNIT_ASSERT_EQUAL(
					String("<a href=\"http://swift.im/\">http://swift.im/</a>"),
					result);
		}


		void testLinkify_BareURL() {
			String result = Linkify::linkify("http://swift.im");

			CPPUNIT_ASSERT_EQUAL(
					String("<a href=\"http://swift.im\">http://swift.im</a>"),
					result);
		}

		void testLinkify_URLSurroundedByWhitespace() {
			String result = Linkify::linkify("Foo http://swift.im/blog Bar");

			CPPUNIT_ASSERT_EQUAL(
					String("Foo <a href=\"http://swift.im/blog\">http://swift.im/blog</a> Bar"),
					result);
		}

		void testLinkify_MultipleURLs() {
			String result = Linkify::linkify("Foo http://swift.im/blog Bar http://el-tramo.be/about Baz");

			CPPUNIT_ASSERT_EQUAL(
					String("Foo <a href=\"http://swift.im/blog\">http://swift.im/blog</a> Bar <a href=\"http://el-tramo.be/about\">http://el-tramo.be/about</a> Baz"),
					result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkifyTest);
