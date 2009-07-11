#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/RawXMLPayloadParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"

using namespace Swift;

class RawXMLPayloadParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RawXMLPayloadParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		RawXMLPayloadParserTest() {}

		void testParse() {
			RawXMLPayloadParser testling;
			PayloadParserTester parser(&testling);

			String xml = 
				"<foo foo-attr=\"foo-val\" xmlns=\"foo-ns\">"
					"<bar bar-attr=\"bar-val\" xmlns=\"bar-ns\"/>"
					"<baz baz-attr=\"baz-val\" xmlns=\"baz-ns\"/>"
				"</foo>";
			CPPUNIT_ASSERT(parser.parse(xml));

			RawXMLPayload* payload = dynamic_cast<RawXMLPayload*>(testling.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(xml, payload->getRawXML());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(RawXMLPayloadParserTest);
