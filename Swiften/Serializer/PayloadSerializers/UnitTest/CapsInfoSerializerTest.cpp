#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/PayloadSerializers/CapsInfoSerializer.h"

using namespace Swift;

class CapsInfoSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(CapsInfoSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		CapsInfoSerializerTest() {}

		void testSerialize() {
			CapsInfoSerializer testling;
			boost::shared_ptr<CapsInfo> priority(new CapsInfo("http://swift.im", "myversion", "sha-1"));

			CPPUNIT_ASSERT_EQUAL(String("<c hash=\"sha-1\" node=\"http://swift.im\" ver=\"myversion\" xmlns=\"http://jabber.org/protocol/caps\"/>"), testling.serialize(priority));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(CapsInfoSerializerTest);
