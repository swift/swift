/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/SerializingParser.h>
#include <Swiften/Parser/UnitTest/StanzaParserTester.h>

using namespace Swift;

class SerializingParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SerializingParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_Empty);
		CPPUNIT_TEST(testParse_ToplevelCharacterData);
		CPPUNIT_TEST_SUITE_END();

	public:
		SerializingParserTest() {}

		void testParse() {
			SerializingParser testling;
			ParserTester<SerializingParser> parser(&testling);

			CPPUNIT_ASSERT(parser.parse(
				"<message type=\"chat\" to=\"me@foo.com\">"
					"<body>Hello&lt;&amp;World</body>"
					"<html xmlns=\"http://www.w3.org/1999/xhtml\">"
						"foo<b>bar</b>baz"
					"</html>"
				"</message>"));

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<message to=\"me@foo.com\" type=\"chat\">"
					"<body>Hello&lt;&amp;World</body>"
					"<html xmlns=\"http://www.w3.org/1999/xhtml\">foo<b xmlns=\"http://www.w3.org/1999/xhtml\">bar</b>baz</html>"
				"</message>"), testling.getResult());
		}

		void testParse_Empty() {
			SerializingParser testling;

			CPPUNIT_ASSERT_EQUAL(std::string(""), testling.getResult());
		}

		void testParse_ToplevelCharacterData() {
			SerializingParser testling;
			
			AttributeMap attributes;
			testling.handleCharacterData("foo");
			testling.handleStartElement("message", "", attributes);
			testling.handleEndElement("message", "");
			testling.handleCharacterData("bar");

			CPPUNIT_ASSERT_EQUAL(std::string("<message/>"), testling.getResult());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SerializingParserTest);
