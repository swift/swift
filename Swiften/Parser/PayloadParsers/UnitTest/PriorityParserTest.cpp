/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/PriorityParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h"

using namespace Swift;

class PriorityParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(PriorityParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		PriorityParserTest() {}

		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<priority>-120</priority>"));

			Priority* payload = dynamic_cast<Priority*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(-120, payload->getPriority());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PriorityParserTest);
