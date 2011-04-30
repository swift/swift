/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/StreamInitiationSerializer.h>

using namespace Swift;

class StreamInitiationSerializerTest : public CppUnit::TestFixture{
		CPPUNIT_TEST_SUITE(StreamInitiationSerializerTest);
		CPPUNIT_TEST(testSerialize_Request);
		CPPUNIT_TEST(testSerialize_Response);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize_Request() {
			StreamInitiationSerializer testling;
			boost::shared_ptr<StreamInitiation> streamInitiation(new StreamInitiation());
			StreamInitiationFileInfo fileInfo("test.txt", "This is info about the file.", 1022);
			streamInitiation->setID("a0");
			streamInitiation->setFileInfo(fileInfo);
			streamInitiation->addProvidedMethod("http://jabber.org/protocol/bytestreams");
			streamInitiation->addProvidedMethod("jabber:iq:oob");
			streamInitiation->addProvidedMethod("http://jabber.org/protocol/ibb");

			CPPUNIT_ASSERT_EQUAL(std::string(
					"<si id=\"a0\" profile=\"http://jabber.org/protocol/si/profile/file-transfer\" xmlns=\"http://jabber.org/protocol/si\">"
						"<file name=\"test.txt\" size=\"1022\" xmlns=\"http://jabber.org/protocol/si/profile/file-transfer\">"
							"<desc>This is info about the file.</desc>"
						"</file>"
						"<feature xmlns=\"http://jabber.org/protocol/feature-neg\">"
							"<x type=\"form\" xmlns=\"jabber:x:data\">"
								"<field type=\"list-single\" var=\"stream-method\">"
									"<option><value>http://jabber.org/protocol/bytestreams</value></option>"
									"<option><value>jabber:iq:oob</value></option>"
									"<option><value>http://jabber.org/protocol/ibb</value></option>"
								"</field>"
							"</x>"
						"</feature>"
					"</si>"
					), testling.serialize(streamInitiation));
		}

		void testSerialize_Response() {
			StreamInitiationSerializer testling;
			boost::shared_ptr<StreamInitiation> streamInitiation(new StreamInitiation());
			streamInitiation->setRequestedMethod("http://jabber.org/protocol/bytestreams");

			CPPUNIT_ASSERT_EQUAL(std::string(
					"<si profile=\"http://jabber.org/protocol/si/profile/file-transfer\" xmlns=\"http://jabber.org/protocol/si\">"
						"<feature xmlns=\"http://jabber.org/protocol/feature-neg\">"
							"<x type=\"submit\" xmlns=\"jabber:x:data\">"
								"<field type=\"list-single\" var=\"stream-method\">"
									"<value>http://jabber.org/protocol/bytestreams</value>"
								"</field>"
							"</x>"
						"</feature>"
					"</si>"
					), testling.serialize(streamInitiation));
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamInitiationSerializerTest);
