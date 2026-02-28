/*
 * Copyright (c) 2012-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/lexical_cast.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/URL.h>

using namespace Swift;

class URLTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(URLTest);
        CPPUNIT_TEST(testFromString);
        CPPUNIT_TEST(testFromString_WithoutPath);
        CPPUNIT_TEST(testFromString_WithRootPath);
        CPPUNIT_TEST(testFromString_WithPort);
        CPPUNIT_TEST(testFromString_WithPortOnePartPath);
        CPPUNIT_TEST(testFromString_WithPortWithoutPath);
        CPPUNIT_TEST(testFromString_WithUserInfo);
        CPPUNIT_TEST(testFromString_NonASCIIHost);
        CPPUNIT_TEST(testFromString_NonASCIIPath);
        CPPUNIT_TEST(testFromString_IPv4Address);
        CPPUNIT_TEST(testFromString_IPv4AddressWithPort);
        CPPUNIT_TEST(testFromString_IPv6Address);
        CPPUNIT_TEST(testFromString_IPv6AddressWithPort);
        CPPUNIT_TEST(testToString);
        CPPUNIT_TEST(testToString_WithPort);
        CPPUNIT_TEST(test_FromString_ToString_IPv6RFC2732);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testFromString() {
            URL url = URL::fromString("http://foo.bar/baz/bam");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
            CPPUNIT_ASSERT(!url.getPort());
            CPPUNIT_ASSERT_EQUAL(std::string("/baz/bam"), url.getPath());
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
            CPPUNIT_ASSERT_EQUAL(std::string("/"), url.getPath());
        }

        void testFromString_WithPort() {
            URL url = URL::fromString("http://foo.bar:1234/baz/bam");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(1234), *url.getPort());
            CPPUNIT_ASSERT_EQUAL(std::string("/baz/bam"), url.getPath());
        }

        void testFromString_WithPortOnePartPath() {
            URL url = URL::fromString("http://foo.bar:11440/http-bind/");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(11440), *url.getPort());
            CPPUNIT_ASSERT_EQUAL(std::string("/http-bind/"), url.getPath());
        }

        void testFromString_WithPortWithoutPath() {
            URL url = URL::fromString("http://foo.bar:1234");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(1234), *url.getPort());
            CPPUNIT_ASSERT_EQUAL(std::string(""), url.getPath());
        }

        void testFromString_WithUserInfo() {
            URL url = URL::fromString("http://user:pass@foo.bar/baz/bam");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("foo.bar"), url.getHost());
            CPPUNIT_ASSERT_EQUAL(std::string("/baz/bam"), url.getPath());
        }

        void testFromString_NonASCIIHost() {
            URL url = URL::fromString("http://www.tron%C3%A7on.be/baz/bam");

            CPPUNIT_ASSERT_EQUAL(std::string("www.tron\xc3\xa7on.be"), url.getHost());
        }

        void testFromString_NonASCIIPath() {
            URL url = URL::fromString("http://foo.bar/baz/tron%C3%A7on/bam");

            CPPUNIT_ASSERT_EQUAL(std::string("/baz/tron\xc3\xa7on/bam"), url.getPath());
        }

        void testFromString_IPv4Address() {
            URL url = URL::fromString("http://127.0.0.1/foobar");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("127.0.0.1"), url.getHost());
            CPPUNIT_ASSERT_EQUAL(std::string("/foobar"), url.getPath());
        }

        void testFromString_IPv4AddressWithPort() {
            URL url = URL::fromString("http://127.0.0.1:12345/foobar");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("127.0.0.1"), url.getHost());
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(12345), url.getPort().get_value_or(0));
            CPPUNIT_ASSERT_EQUAL(std::string("/foobar"), url.getPath());
        }

        void testFromString_IPv6Address() {
            URL url = URL::fromString("http://[fdf8:f53b:82e4::53]/foobar");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("fdf8:f53b:82e4::53"), url.getHost());
        }

        void testFromString_IPv6AddressWithPort() {
            URL url = URL::fromString("http://[fdf8:f53b:82e4::53]:12435/foobar");

            CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
            CPPUNIT_ASSERT_EQUAL(std::string("fdf8:f53b:82e4::53"), url.getHost());
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(12435), url.getPort().get_value_or(0));
        }

        void test_FromString_ToString_IPv6RFC2732() {
            {
                const char* testVector = "http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html";
                URL url = URL::fromString(testVector);

                CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
                CPPUNIT_ASSERT_EQUAL(std::string("FEDC:BA98:7654:3210:FEDC:BA98:7654:3210"), url.getHost());
                CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(80), url.getPort().get_value_or(2));
                CPPUNIT_ASSERT_EQUAL(std::string("/index.html"), url.getPath());

                CPPUNIT_ASSERT_EQUAL(std::string(testVector), url.toString());
            }

            {
                const char* testVector = "http://[1080:0:0:0:8:800:200C:417A]/index.html";
                URL url = URL::fromString(testVector);

                CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
                CPPUNIT_ASSERT_EQUAL(std::string("1080:0:0:0:8:800:200C:417A"), url.getHost());
                CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(2), url.getPort().get_value_or(2));
                CPPUNIT_ASSERT_EQUAL(std::string("/index.html"), url.getPath());

                CPPUNIT_ASSERT_EQUAL(std::string(testVector), url.toString());
            }

            {
                const char* testVector = "http://[3ffe:2a00:100:7031::1]";
                URL url = URL::fromString(testVector);

                CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
                CPPUNIT_ASSERT_EQUAL(std::string("3ffe:2a00:100:7031::1"), url.getHost());
                CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(2), url.getPort().get_value_or(2));
                CPPUNIT_ASSERT_EQUAL(std::string(""), url.getPath());

                CPPUNIT_ASSERT_EQUAL(std::string(testVector), url.toString());
            }

            {
                const char* testVector = "http://[1080::8:800:200C:417A]/foo";
                URL url = URL::fromString(testVector);

                CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
                CPPUNIT_ASSERT_EQUAL(std::string("1080::8:800:200C:417A"), url.getHost());
                CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(2), url.getPort().get_value_or(2));
                CPPUNIT_ASSERT_EQUAL(std::string("/foo"), url.getPath());

                CPPUNIT_ASSERT_EQUAL(std::string(testVector), url.toString());
            }

            {
                const char* testVector = "http://[::192.9.5.5]/ipng";
                URL url = URL::fromString(testVector);

                CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
                CPPUNIT_ASSERT_EQUAL(std::string("::192.9.5.5"), url.getHost());
                CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(2), url.getPort().get_value_or(2));
                CPPUNIT_ASSERT_EQUAL(std::string("/ipng"), url.getPath());

                CPPUNIT_ASSERT_EQUAL(std::string(testVector), url.toString());
            }

            {
                const char* testVector = "http://[::FFFF:129.144.52.38]:80/index.html";
                URL url = URL::fromString(testVector);

                CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
                CPPUNIT_ASSERT_EQUAL(std::string("::FFFF:129.144.52.38"), url.getHost());
                CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(80), url.getPort().get_value_or(2));
                CPPUNIT_ASSERT_EQUAL(std::string("/index.html"), url.getPath());

                CPPUNIT_ASSERT_EQUAL(std::string(testVector), url.toString());
            }

            {
                const char* testVector = "http://[2010:836B:4179::836B:4179]";
                URL url = URL::fromString(testVector);

                CPPUNIT_ASSERT_EQUAL(std::string("http"), url.getScheme());
                CPPUNIT_ASSERT_EQUAL(std::string("2010:836B:4179::836B:4179"), url.getHost());
                CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(2), url.getPort().get_value_or(2));
                CPPUNIT_ASSERT_EQUAL(std::string(), url.getPath());

                CPPUNIT_ASSERT_EQUAL(std::string(testVector), url.toString());
            }
        }

        void testToString() {
            CPPUNIT_ASSERT_EQUAL(std::string("http://foo.bar/baz/bam"), URL("http", "foo.bar", "/baz/bam").toString());
        }

        void testToString_WithPort() {
            CPPUNIT_ASSERT_EQUAL(std::string("http://foo.bar:1234/baz/bam"), URL("http", "foo.bar", 1234, "/baz/bam").toString());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(URLTest);
