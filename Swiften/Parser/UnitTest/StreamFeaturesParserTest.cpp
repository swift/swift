/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/StreamFeaturesParser.h>
#include <Swiften/Parser/UnitTest/ElementParserTester.h>

using namespace Swift;

class StreamFeaturesParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(StreamFeaturesParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_Empty);
		CPPUNIT_TEST_SUITE_END();

	public:
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
					"<sm xmlns='urn:xmpp:sm:2'/>"
					"<session xmlns=\"urn:ietf:params:xml:ns:xmpp-session\"/>"
					"<ver xmlns=\"urn:xmpp:features:rosterver\"/>"
				"</stream:features>"));

			StreamFeatures::ref element = boost::dynamic_pointer_cast<StreamFeatures>(testling.getElement());
			CPPUNIT_ASSERT(element->hasStartTLS());
			CPPUNIT_ASSERT(element->hasSession());
			CPPUNIT_ASSERT(element->hasResourceBind());
			CPPUNIT_ASSERT(element->hasCompressionMethod("zlib"));
			CPPUNIT_ASSERT(element->hasCompressionMethod("lzw"));
			CPPUNIT_ASSERT(element->hasAuthenticationMechanisms());
			CPPUNIT_ASSERT(element->hasAuthenticationMechanism("DIGEST-MD5"));
			CPPUNIT_ASSERT(element->hasAuthenticationMechanism("PLAIN"));
			CPPUNIT_ASSERT(element->hasStreamManagement());
			CPPUNIT_ASSERT(element->hasRosterVersioning());
		}

		void testParse_Empty() {
			StreamFeaturesParser testling;
			ElementParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<stream:features xmlns:stream='http://etherx.jabber.org/streams'/>"));

			StreamFeatures::ref element = boost::dynamic_pointer_cast<StreamFeatures>(testling.getElement());
			CPPUNIT_ASSERT(!element->hasStartTLS());
			CPPUNIT_ASSERT(!element->hasSession());
			CPPUNIT_ASSERT(!element->hasResourceBind());
			CPPUNIT_ASSERT(!element->hasAuthenticationMechanisms());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamFeaturesParserTest);
