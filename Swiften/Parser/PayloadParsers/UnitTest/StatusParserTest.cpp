#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/StatusParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"

using namespace Swift;

class StatusParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StatusParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		StatusParserTest() {}

		void testParse() {
			StatusParser testling;
			PayloadParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<status>foo<baz>bar</baz>fum</status>"));

			Status* payload = dynamic_cast<Status*>(testling.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(String("foobarfum"), payload->getText());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StatusParserTest);
