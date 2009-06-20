#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/VCardUpdateParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h"

using namespace Swift;

class VCardUpdateParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(VCardUpdateParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		VCardUpdateParserTest() {}

		void testParse() {
			VCardUpdateParser testling;
			PayloadParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse(
				"<x xmlns='vcard-temp:x:update'>"
					"<photo>sha1-hash-of-image</photo>"
				"</x>"));

			VCardUpdate* payload = dynamic_cast<VCardUpdate*>(testling.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(String("sha1-hash-of-image"), payload->getPhotoHash());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardUpdateParserTest);
