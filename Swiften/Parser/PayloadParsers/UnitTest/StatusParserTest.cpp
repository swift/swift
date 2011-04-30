/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/StatusParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class StatusParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StatusParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		StatusParserTest() {}

		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<status>foo<baz>bar</baz>fum</status>"));

			Status* payload = dynamic_cast<Status*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(std::string("foobarfum"), payload->getText());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StatusParserTest);
