/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XMPPSerializer.h>

using namespace Swift;

class XMPPSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(XMPPSerializerTest);
        CPPUNIT_TEST(testSerializeHeader_Client);
        CPPUNIT_TEST(testSerializeHeader_Component);
        CPPUNIT_TEST(testSerializeHeader_Server);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            payloadSerializerCollection = new PayloadSerializerCollection();
        }

        void tearDown() {
            delete payloadSerializerCollection;
        }

        void testSerializeHeader_Client() {
            std::shared_ptr<XMPPSerializer> testling(createSerializer(ClientStreamType));
            ProtocolHeader protocolHeader;
            protocolHeader.setFrom("bla@foo.com");
            protocolHeader.setTo("foo.com");
            protocolHeader.setID("myid");
            protocolHeader.setVersion("0.99");

            CPPUNIT_ASSERT_EQUAL(std::string("<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" from=\"bla@foo.com\" to=\"foo.com\" id=\"myid\" version=\"0.99\">"), testling->serializeHeader(protocolHeader));
        }

        void testSerializeHeader_Component() {
            std::shared_ptr<XMPPSerializer> testling(createSerializer(ComponentStreamType));
            ProtocolHeader protocolHeader;
            protocolHeader.setFrom("bla@foo.com");
            protocolHeader.setTo("foo.com");
            protocolHeader.setID("myid");
            protocolHeader.setVersion("0.99");

            CPPUNIT_ASSERT_EQUAL(std::string("<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:component:accept\" xmlns:stream=\"http://etherx.jabber.org/streams\" from=\"bla@foo.com\" to=\"foo.com\" id=\"myid\" version=\"0.99\">"), testling->serializeHeader(protocolHeader));
        }

        void testSerializeHeader_Server() {
            std::shared_ptr<XMPPSerializer> testling(createSerializer(ServerStreamType));
            ProtocolHeader protocolHeader;
            protocolHeader.setFrom("bla@foo.com");
            protocolHeader.setTo("foo.com");
            protocolHeader.setID("myid");
            protocolHeader.setVersion("0.99");

            CPPUNIT_ASSERT_EQUAL(std::string("<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:server\" xmlns:stream=\"http://etherx.jabber.org/streams\" from=\"bla@foo.com\" to=\"foo.com\" id=\"myid\" version=\"0.99\">"), testling->serializeHeader(protocolHeader));
        }

    private:
        XMPPSerializer* createSerializer(StreamType type) {
            return new XMPPSerializer(payloadSerializerCollection, type, false);
        }

    private:
        PayloadSerializerCollection* payloadSerializerCollection;
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPSerializerTest);
