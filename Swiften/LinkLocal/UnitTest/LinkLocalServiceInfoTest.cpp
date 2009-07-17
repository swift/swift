#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

using namespace Swift;

class LinkLocalServiceInfoTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalServiceInfoTest);
		CPPUNIT_TEST(testGetTXTRecord);
		CPPUNIT_TEST_SUITE_END();

	public:
		LinkLocalServiceInfoTest() {}

		void testGetTXTRecord() {
			LinkLocalServiceInfo info;
			info.setFirstName("Remko");
			info.setLastName("Tron\xc3\xe7on");
			info.setStatus(LinkLocalServiceInfo::Away);

			CPPUNIT_ASSERT_EQUAL(ByteArray("\x09txtvers=1\x09" + String("1st=Remko\x0dlast=Tron\xc3\xe7on\x0bstatus=away")), info.toTXTRecord());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalServiceInfoTest);
