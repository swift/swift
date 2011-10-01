/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Parser/BOSHBodyExtractor.h>

using namespace Swift;

class BOSHBodyExtractorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(BOSHBodyExtractorTest);
		CPPUNIT_TEST(testGetBody);
		CPPUNIT_TEST(testGetBody_EmptyContent);
		CPPUNIT_TEST(testGetBody_EmptyContent2);
		CPPUNIT_TEST(testGetBody_EmptyElementEmptyContent);
		CPPUNIT_TEST(testGetBody_InvalidStartTag);
		CPPUNIT_TEST(testGetBody_InvalidStartTag2);
		CPPUNIT_TEST(testGetBody_IncompleteStartTag);
		CPPUNIT_TEST(testGetBody_InvalidEndTag);
		CPPUNIT_TEST(testGetBody_InvalidEndTag2);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetBody() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<body a1='a\"1' a2=\"a'2\" boo='bar'   >"
						"foo <message> <body> bar"
					"</body   >  "));

			CPPUNIT_ASSERT(testling.getBody());
			CPPUNIT_ASSERT_EQUAL(std::string("a\"1"), testling.getBody()->attributes.getAttribute("a1"));
			CPPUNIT_ASSERT_EQUAL(std::string("foo <message> <body> bar"), testling.getBody()->content);
		}

		void testGetBody_EmptyContent() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<body foo='bar'/>"));

			CPPUNIT_ASSERT(testling.getBody());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getBody()->attributes.getAttribute("foo"));
			CPPUNIT_ASSERT(testling.getBody()->content.empty());
		}

		void testGetBody_EmptyContent2() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<body foo='bar'></body>"));

			CPPUNIT_ASSERT(testling.getBody());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getBody()->attributes.getAttribute("foo"));
			CPPUNIT_ASSERT(testling.getBody()->content.empty());
		}

		void testGetBody_EmptyElementEmptyContent() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<body/>"));

			CPPUNIT_ASSERT(testling.getBody());
		}

		void testGetBody_InvalidStartTag() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<bodi></body>"));

			CPPUNIT_ASSERT(!testling.getBody());
		}

		void testGetBody_InvalidStartTag2() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<bodyy></body>"));

			CPPUNIT_ASSERT(!testling.getBody());
		}

		void testGetBody_IncompleteStartTag() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<body"));

			CPPUNIT_ASSERT(!testling.getBody());
		}

		void testGetBody_InvalidEndTag() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<body></bodi>"));

			CPPUNIT_ASSERT(!testling.getBody());
		}

		void testGetBody_InvalidEndTag2() {
			BOSHBodyExtractor testling(&parserFactory, createByteArray(
					"<body><b/body>"));

			CPPUNIT_ASSERT(!testling.getBody());
		}

	private:
		PlatformXMLParserFactory parserFactory;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BOSHBodyExtractorTest);
