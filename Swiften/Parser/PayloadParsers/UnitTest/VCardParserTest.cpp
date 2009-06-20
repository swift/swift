#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/VCardParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"

using namespace Swift;

class VCardParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(VCardParserTest);
		CPPUNIT_TEST(testParse_Photo);
		CPPUNIT_TEST_SUITE_END();

	public:
		VCardParserTest() {}

		void testParse_Photo() {
			VCardParser testling;
			PayloadParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse(
				"<vCard xmlns='vcard-temp'>"
					"<PHOTO>"
						"<TYPE>image/jpeg</TYPE>"
						"<BINVAL>"
							"QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ej"
							"EyMzQ1Njc4OTA="
						"</BINVAL>"
					"</PHOTO>"
				"</vCard>"));

			VCard* payload = dynamic_cast<VCard*>(testling.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(String("image/jpeg"), payload->getPhotoType());
			CPPUNIT_ASSERT_EQUAL(ByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890"), payload->getPhoto());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardParserTest);
