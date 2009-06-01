#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/ErrorParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"

using namespace Swift;

class ErrorParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(ErrorParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		ErrorParserTest() {}

		void testParse() {
			ErrorParser testling;
			PayloadParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse(
				"<error type=\"modify\">"
					"<bad-request xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/>"
					"<text xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\">boo</text>"
				"</error>"));

			Error* payload = dynamic_cast<Error*>(testling.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(Error::BadRequest, payload->getCondition());
			CPPUNIT_ASSERT_EQUAL(Error::Modify, payload->getType());
			CPPUNIT_ASSERT_EQUAL(String("boo"), payload->getText());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ErrorParserTest);
