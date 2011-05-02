/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/XMPPSerializer.h>
#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Elements/ProtocolHeader.h>

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
			boost::shared_ptr<XMPPSerializer> testling(createSerializer(ClientStreamType));
			ProtocolHeader protocolHeader;
			protocolHeader.setFrom("bla@foo.com");
			protocolHeader.setTo("foo.com");
			protocolHeader.setID("myid");
			protocolHeader.setVersion("0.99");

			CPPUNIT_ASSERT_EQUAL(std::string("<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" from=\"bla@foo.com\" to=\"foo.com\" id=\"myid\" version=\"0.99\">"), testling->serializeHeader(protocolHeader));
		}

		void testSerializeHeader_Component() {
			boost::shared_ptr<XMPPSerializer> testling(createSerializer(ComponentStreamType));
			ProtocolHeader protocolHeader;
			protocolHeader.setFrom("bla@foo.com");
			protocolHeader.setTo("foo.com");
			protocolHeader.setID("myid");
			protocolHeader.setVersion("0.99");

			CPPUNIT_ASSERT_EQUAL(std::string("<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:component:accept\" xmlns:stream=\"http://etherx.jabber.org/streams\" from=\"bla@foo.com\" to=\"foo.com\" id=\"myid\" version=\"0.99\">"), testling->serializeHeader(protocolHeader));
		}

		void testSerializeHeader_Server() {
			boost::shared_ptr<XMPPSerializer> testling(createSerializer(ServerStreamType));
			ProtocolHeader protocolHeader;
			protocolHeader.setFrom("bla@foo.com");
			protocolHeader.setTo("foo.com");
			protocolHeader.setID("myid");
			protocolHeader.setVersion("0.99");

			CPPUNIT_ASSERT_EQUAL(std::string("<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:server\" xmlns:stream=\"http://etherx.jabber.org/streams\" from=\"bla@foo.com\" to=\"foo.com\" id=\"myid\" version=\"0.99\">"), testling->serializeHeader(protocolHeader));
		}

	private:
		XMPPSerializer* createSerializer(StreamType type) {
			return new XMPPSerializer(payloadSerializerCollection, type);
		}

	private:
		PayloadSerializerCollection* payloadSerializerCollection;
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPSerializerTest);
