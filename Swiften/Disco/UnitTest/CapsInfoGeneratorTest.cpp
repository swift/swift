#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Disco/CapsInfoGenerator.h"

using namespace Swift;

class CapsInfoGeneratorTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(CapsInfoGeneratorTest);
		CPPUNIT_TEST(testGenerate_XEP0115SimpleExample);
		CPPUNIT_TEST_SUITE_END();

	public:
		CapsInfoGeneratorTest() {}

		void testGenerate_XEP0115SimpleExample() {
			DiscoInfo discoInfo;
			discoInfo.addIdentity(DiscoInfo::Identity("Exodus 0.9.1", "client", "pc"));
			discoInfo.addFeature("http://jabber.org/protocol/disco#items");
			discoInfo.addFeature("http://jabber.org/protocol/caps");
			discoInfo.addFeature("http://jabber.org/protocol/disco#info");
			discoInfo.addFeature("http://jabber.org/protocol/muc");

			CapsInfoGenerator testling("http://code.google.com/p/exodus");
			CapsInfo result = testling.generateCapsInfo(discoInfo);

			CPPUNIT_ASSERT_EQUAL(String("http://code.google.com/p/exodus"), result.getNode());
			CPPUNIT_ASSERT_EQUAL(String("sha-1"), result.getHash());
			CPPUNIT_ASSERT_EQUAL(String("QgayPKawpkPSDYmwT/WM94uAlu0="), result.getVersion());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(CapsInfoGeneratorTest);
