#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/BodyParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"

using namespace Swift;

class BodyParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(BodyParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		BodyParserTest() {}

		void testParse() {
			BodyParser testling;
			PayloadParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<body>foo<baz>bar</baz>fum</body>"));

			Body* payload = dynamic_cast<Body*>(testling.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(String("foobarfum"), payload->getText());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(BodyParserTest);
