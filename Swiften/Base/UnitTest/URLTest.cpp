/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/URL.h>
#include <boost/lexical_cast.hpp>

using namespace Swift;

class URLTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(URLTest);
		CPPUNIT_TEST(testFromString);
		CPPUNIT_TEST(testFromString_WithoutPath);
		CPPUNIT_TEST(testFromString_WithRootPath);
		CPPUNIT_TEST(testFromString_WithPort);
		CPPUNIT_TEST(testFromString_WithPortWithoutPath);
		CPPUNIT_TEST(testFromString_WithUserInfo);
		CPPUNIT_TEST(testFromString_NonASCIIHost);
		CPPUNIT_TEST(testFromString_NonASCIIPath);
		CPPUNIT_TEST(testToString);
		CPPUNIT_TEST(testToString_WithPort);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testFromString() {
			URL url = URL::fromString("http://foo.bar/baz/bam");
			
			CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
			CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
			CPPUNIT_ASSERT(!url.getPort());
			CPPUNIT_ASSERT_EQUAL(std::string("baz/bam"), url.getPath());
		}

		void testFromString_WithoutPath() {
			URL url = URL::fromString("http://foo.bar");
			
			CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
			CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
			CPPUNIT_ASSERT(!url.getPort());
			CPPUNIT_ASSERT_EQUAL(std::string(""), url.getPath());
		}

		void testFromString_WithRootPath() {
			URL url = URL::fromString("http://foo.bar/");
			
			CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
			CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
			CPPUNIT_ASSERT(!url.getPort());
			CPPUNIT_ASSERT_EQUAL(std::string(""), url.getPath());
		}

		void testFromString_WithPort() {
			URL url = URL::fromString("http://foo.bar:1234/baz/bam");
			
			CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
			CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
			CPPUNIT_ASSERT_EQUAL(1234, *url.getPort());
			CPPUNIT_ASSERT_EQUAL(std::string("baz/bam"), url.getPath());
		}

		void testFromString_WithPortWithoutPath() {
			URL url = URL::fromString("http://foo.bar:1234");
			
			CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
			CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
			CPPUNIT_ASSERT_EQUAL(1234, *url.getPort());
			CPPUNIT_ASSERT_EQUAL(std::string(""), url.getPath());
		}

		void testFromString_WithUserInfo() {
			URL url = URL::fromString("http://user:pass@foo.bar/baz/bam");
			
			CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
			CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
			CPPUNIT_ASSERT_EQUAL(std::string("baz/bam"), url.getPath());
		}

		void testFromString_NonASCIIHost() {
			URL url = URL::fromString("http://www.tron%C3%A7on.be/baz/bam");

			CPPUNIT_ASSERT_EQUAL(std::string("www.tron\xc3\xa7on.be"), url.getHost());
		}

		void testFromString_NonASCIIPath() {
			URL url = URL::fromString("http://foo.bar/baz/tron%C3%A7on/bam");
			
			CPPUNIT_ASSERT_EQUAL(std::string("baz/tron\xc3\xa7on/bam"), url.getPath());
		}

		void testToString() {
			CPPUNIT_ASSERT_EQUAL(std::string("http://foo.bar/baz/bam"), URL("http", "foo.bar", "/baz/bam").toString());
		}

		void testToString_WithPort() {
			CPPUNIT_ASSERT_EQUAL(std::string("http://foo.bar:1234/baz/bam"), URL("http", "foo.bar", 1234, "/baz/bam").toString());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(URLTest);
