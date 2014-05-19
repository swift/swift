/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/smart_ptr/make_shared.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

class PubSubItemSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PubSubItemSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerializeEmptyID);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize() {
			PubSubItemSerializer serializer(&serializers);

			boost::shared_ptr<RawXMLPayload> payload(boost::make_shared<RawXMLPayload>());
			payload->setRawXML("<payload xmlns=\"tmp\"/>");

			boost::shared_ptr<PubSubItem> item(boost::make_shared<PubSubItem>());
			item->addData(payload);
			item->setID("pubsub-item-1");

			std::string expectedResult = 
				"<item id=\"pubsub-item-1\" xmlns=\"http://jabber.org/protocol/pubsub\">"
					"<payload xmlns=\"tmp\"/>"
				"</item>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(item));
		}

		void testSerializeEmptyID() {
			PubSubItemSerializer serializer(&serializers);

			boost::shared_ptr<PubSubItem> item(boost::make_shared<PubSubItem>());

			std::string expectedResult = 
				"<item xmlns=\"http://jabber.org/protocol/pubsub\"/>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(item));
		}

	private:
		FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PubSubItemSerializerTest);
