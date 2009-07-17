#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

using namespace Swift;

class LinkLocalServiceInfoTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalServiceInfoTest);
		CPPUNIT_TEST(testGetTXTRecord);
		CPPUNIT_TEST(testCreateFromTXTRecord);
		CPPUNIT_TEST(testCreateFromTXTRecord_InvalidSize);
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

		void testCreateFromTXTRecord() {
			LinkLocalServiceInfo info = LinkLocalServiceInfo::createFromTXTRecord(ByteArray("\x09txtvers=1\x09" + String("1st=Remko\x0dlast=Tron\xc3\xe7on\x0bstatus=away")));

			CPPUNIT_ASSERT_EQUAL(String("Remko"), info.getFirstName());
			CPPUNIT_ASSERT_EQUAL(String("Tron\xc3\xe7on"), info.getLastName());
			CPPUNIT_ASSERT_EQUAL(LinkLocalServiceInfo::Away, info.getStatus());
		}

		void testCreateFromTXTRecord_InvalidSize() {
			LinkLocalServiceInfo info = LinkLocalServiceInfo::createFromTXTRecord(ByteArray("\x10last=a"));

			CPPUNIT_ASSERT_EQUAL(String("a"), info.getLastName());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalServiceInfoTest);
