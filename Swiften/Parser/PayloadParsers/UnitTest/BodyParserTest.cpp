/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/BodyParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class BodyParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(BodyParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		BodyParserTest() {}

		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<body>foo<baz>bar</baz>fum</body>"));

			Body* payload = dynamic_cast<Body*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(std::string("foobarfum"), payload->getText());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(BodyParserTest);
