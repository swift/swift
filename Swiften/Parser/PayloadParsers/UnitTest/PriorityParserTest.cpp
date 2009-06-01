#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/PriorityParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"

using namespace Swift;

class PriorityParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(PriorityParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		PriorityParserTest() {}

		void testParse() {
			PriorityParser testling;
			PayloadParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<priority>-120</priority>"));

			Priority* payload = dynamic_cast<Priority*>(testling.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(-120, payload->getPriority());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PriorityParserTest);
