#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/PayloadSerializers/DiscoInfoSerializer.h"

using namespace Swift;

class DiscoInfoSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(DiscoInfoSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		DiscoInfoSerializerTest() {}

		void testSerialize() {
			DiscoInfoSerializer testling;
			boost::shared_ptr<DiscoInfo> discoInfo(new DiscoInfo());
			discoInfo->addIdentity(DiscoInfo::Identity("Swift", "client", "pc"));
			discoInfo->addIdentity(DiscoInfo::Identity("Vlug", "client", "pc", "nl"));
			discoInfo->addFeature("http://jabber.org/protocol/caps");
			discoInfo->addFeature("http://jabber.org/protocol/disco#info");
			discoInfo->setNode("http://swift.im#bla");

			String expectedResult = 
				"<query node=\"http://swift.im#bla\" xmlns=\"http://jabber.org/protocol/disco#info\">"
					"<identity category=\"client\" name=\"Swift\" type=\"pc\"/>"
					"<identity category=\"client\" name=\"Vlug\" type=\"pc\" xml:lang=\"nl\"/>"
					"<feature var=\"http://jabber.org/protocol/caps\"/>"
					"<feature var=\"http://jabber.org/protocol/disco#info\"/>"
				"</query>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(discoInfo));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(DiscoInfoSerializerTest);
