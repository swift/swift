#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/PayloadSerializers/SecurityLabelSerializer.h"

using namespace Swift;

class SecurityLabelSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SecurityLabelSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		SecurityLabelSerializerTest() {}

		void testSerialize() {
			SecurityLabelSerializer testling;
			boost::shared_ptr<SecurityLabel> securityLabel(new SecurityLabel());
			securityLabel->setDisplayMarking("SECRET");
			securityLabel->setForegroundColor("black");
			securityLabel->setBackgroundColor("red");
			securityLabel->setLabel("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel>");
			securityLabel->addEquivalentLabel("<icismlabel xmlns=\"http://example.gov/IC-ISM/0\" classification=\"S\" ownerProducer=\"USA\" disseminationControls=\"FOUO\"/>");
			securityLabel->addEquivalentLabel("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MRUCAgD9DA9BcXVhIChvYnNvbGV0ZSk=</esssecuritylabel>");

			CPPUNIT_ASSERT_EQUAL(String(
				"<securitylabel xmlns=\"urn:xmpp:sec-label:0\">"
					"<displaymarking bgcolor=\"red\" fgcolor=\"black\">SECRET</displaymarking>"
					"<label>"
						"<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel>"
					"</label>"
					"<equivalentlabel>"
						"<icismlabel xmlns=\"http://example.gov/IC-ISM/0\" classification=\"S\" ownerProducer=\"USA\" disseminationControls=\"FOUO\"/>"
					"</equivalentlabel>"
					"<equivalentlabel>"
						"<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MRUCAgD9DA9BcXVhIChvYnNvbGV0ZSk=</esssecuritylabel>"
					"</equivalentlabel>"
				"</securitylabel>"), testling.serialize(securityLabel));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SecurityLabelSerializerTest);
