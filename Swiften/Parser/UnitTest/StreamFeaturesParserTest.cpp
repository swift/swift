/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
        CPPUNIT_TEST(testParse_AuthenticationHostname);
        CPPUNIT_TEST(testParse_AuthenticationHostnameEmpty);
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

            StreamFeatures::ref element = std::dynamic_pointer_cast<StreamFeatures>(testling.getElement());
            CPPUNIT_ASSERT(element->hasStartTLS());
            CPPUNIT_ASSERT(element->hasSession());
            CPPUNIT_ASSERT(element->hasResourceBind());
            CPPUNIT_ASSERT(element->hasCompressionMethod("zlib"));
            CPPUNIT_ASSERT(element->hasCompressionMethod("lzw"));
            CPPUNIT_ASSERT(element->hasAuthenticationMechanisms());
            CPPUNIT_ASSERT(element->hasAuthenticationMechanism("DIGEST-MD5"));
            CPPUNIT_ASSERT(element->hasAuthenticationMechanism("PLAIN"));
            CPPUNIT_ASSERT(!element->getAuthenticationHostname());
            CPPUNIT_ASSERT(element->hasStreamManagement());
            CPPUNIT_ASSERT(element->hasRosterVersioning());
        }

        void testParse_Empty() {
            StreamFeaturesParser testling;
            ElementParserTester parser(&testling);

            CPPUNIT_ASSERT(parser.parse("<stream:features xmlns:stream='http://etherx.jabber.org/streams'/>"));

            StreamFeatures::ref element = std::dynamic_pointer_cast<StreamFeatures>(testling.getElement());
            CPPUNIT_ASSERT(!element->hasStartTLS());
            CPPUNIT_ASSERT(!element->hasSession());
            CPPUNIT_ASSERT(!element->hasResourceBind());
            CPPUNIT_ASSERT(!element->hasAuthenticationMechanisms());
        }

        void testParse_AuthenticationHostname() {
            StreamFeaturesParser testling;
            ElementParserTester parser(&testling);
            std::string hostname("auth42.us.example.com");

            CPPUNIT_ASSERT(parser.parse(
                "<stream:features xmlns:stream='http://etherx.jabber.org/streams'>"
                    "<mechanisms xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
                        "<mechanism>GSSAPI</mechanism>"
                        "<hostname xmlns=\"urn:xmpp:domain-based-name:1\">auth42.us.example.com</hostname>"
                    "</mechanisms>"
                "</stream:features>"));

            StreamFeatures::ref element = std::dynamic_pointer_cast<StreamFeatures>(testling.getElement());
            CPPUNIT_ASSERT(element->hasAuthenticationMechanism("GSSAPI"));
            CPPUNIT_ASSERT_EQUAL(*element->getAuthenticationHostname(), hostname);
        }

        void testParse_AuthenticationHostnameEmpty() {
            StreamFeaturesParser testling;
            ElementParserTester parser(&testling);

            CPPUNIT_ASSERT(parser.parse(
                "<stream:features xmlns:stream='http://etherx.jabber.org/streams'>"
                    "<mechanisms xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
                        "<mechanism>GSSAPI</mechanism>"
                        "<hostname xmlns=\"urn:xmpp:domain-based-name:1\"></hostname>"
                    "</mechanisms>"
                "</stream:features>"));

            StreamFeatures::ref element = std::dynamic_pointer_cast<StreamFeatures>(testling.getElement());
            CPPUNIT_ASSERT(element->hasAuthenticationMechanism("GSSAPI"));
            CPPUNIT_ASSERT(element->getAuthenticationHostname()->empty());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamFeaturesParserTest);
