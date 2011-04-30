/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/PriorityParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class PriorityParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PriorityParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_Invalid);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<priority>-120</priority>"));

			boost::shared_ptr<Priority> payload = boost::dynamic_pointer_cast<Priority>(parser.getPayload());
			CPPUNIT_ASSERT_EQUAL(-120, payload->getPriority());
		}

		void testParse_Invalid() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<priority>invalid</priority>"));

			boost::shared_ptr<Priority> payload = boost::dynamic_pointer_cast<Priority>(parser.getPayload());
			CPPUNIT_ASSERT_EQUAL(0, payload->getPriority());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PriorityParserTest);
