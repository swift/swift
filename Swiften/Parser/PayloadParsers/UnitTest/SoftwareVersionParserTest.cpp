#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/SoftwareVersionParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"

using namespace Swift;

class SoftwareVersionParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SoftwareVersionParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		SoftwareVersionParserTest() {}

		void testParse() {
			SoftwareVersionParser testling;
			PayloadParserTester parser(&testling);
			
			CPPUNIT_ASSERT(parser.parse(
				"<query xmlns=\"jabber:iq:version\">"
					"<name>myclient</name>"
					"<version>1.0</version>"
					"<os>Mac OS X</os>"
				"</query>"));

			SoftwareVersion* payload = dynamic_cast<SoftwareVersion*>(testling.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(String("myclient"), payload->getName());
			CPPUNIT_ASSERT_EQUAL(String("1.0"), payload->getVersion());
			CPPUNIT_ASSERT_EQUAL(String("Mac OS X"), payload->getOS());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SoftwareVersionParserTest);
