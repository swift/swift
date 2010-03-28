#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/StreamFeaturesParser.h"
#include "Swiften/Parser/UnitTest/ElementParserTester.h"

using namespace Swift;

class StreamFeaturesParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StreamFeaturesParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_Empty);
		CPPUNIT_TEST_SUITE_END();

	public:
		StreamFeaturesParserTest() {}

		void testParse() {
			StreamFeaturesParser testling;
			ElementParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse(
				"<stream:features xmlns:stream='http://etherx.jabber.org/streams'>"
					"<starttls xmlns=\"urn:ietf:params:xml:ns:xmpp-tls\"/>"
					"<compression xmlns=\"http://jabber.org/features/compress\">"
						"<method>zlib</method>"
						"<method>lzw</method>"
					"</compression>"
					"<mechanisms xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
						"<mechanism>DIGEST-MD5</mechanism>"
						"<mechanism>PLAIN</mechanism>"
					"</mechanisms>"
					"<bind xmlns=\"urn:ietf:params:xml:ns:xmpp-bind\"/>"
					"<session xmlns=\"urn:ietf:params:xml:ns:xmpp-session\"/>"
				"</stream:features>"));

			StreamFeatures* element = dynamic_cast<StreamFeatures*>(testling.getElement().get());
			CPPUNIT_ASSERT(element->hasStartTLS());
			CPPUNIT_ASSERT(element->hasSession());
			CPPUNIT_ASSERT(element->hasResourceBind());
			CPPUNIT_ASSERT(element->hasCompressionMethod("zlib"));
			CPPUNIT_ASSERT(element->hasCompressionMethod("lzw"));
			CPPUNIT_ASSERT(element->hasAuthenticationMechanisms());
			CPPUNIT_ASSERT(element->hasAuthenticationMechanism("DIGEST-MD5"));
			CPPUNIT_ASSERT(element->hasAuthenticationMechanism("PLAIN"));
		}

		void testParse_Empty() {
			StreamFeaturesParser testling;
			ElementParserTester parser(&testling);

			parser.parse("<stream:features xmlns:stream='http://etherx.jabber.org/streams'/>");

			StreamFeatures* element = dynamic_cast<StreamFeatures*>(testling.getElement().get());
			CPPUNIT_ASSERT(!element->hasStartTLS());
			CPPUNIT_ASSERT(!element->hasSession());
			CPPUNIT_ASSERT(!element->hasResourceBind());
			CPPUNIT_ASSERT(!element->hasAuthenticationMechanisms());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamFeaturesParserTest);
