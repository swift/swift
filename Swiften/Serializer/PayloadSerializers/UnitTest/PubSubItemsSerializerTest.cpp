/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/smart_ptr/make_shared.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

class PubSubItemsSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PubSubItemsSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerializeEmptyItems);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize() {
			PubSubItemsSerializer serializer(&serializers);

			boost::shared_ptr<RawXMLPayload> payload1(boost::make_shared<RawXMLPayload>());
			payload1->setRawXML("<payload xmlns=\"tmp\"/>");

			boost::shared_ptr<PubSubItem> item1(boost::make_shared<PubSubItem>());
			item1->addData(payload1);
			item1->setID("pubsub-item-1");

			boost::shared_ptr<RawXMLPayload> payload2(boost::make_shared<RawXMLPayload>());
			payload2->setRawXML("<payload xmlns=\"other-tmp\"/>");

			boost::shared_ptr<PubSubItem> item2(boost::make_shared<PubSubItem>());
			item2->addData(payload2);
			item2->setID("pubsub-item-2");

			boost::shared_ptr<PubSubItems> items(boost::make_shared<PubSubItems>());
			items->setNode("test-node");
			items->setSubscriptionID(std::string("sub-id"));
			items->addItem(item1);
			items->addItem(item2);

			std::string expectedResult = 
				"<items node=\"test-node\" subid=\"sub-id\" xmlns=\"http://jabber.org/protocol/pubsub\">"
					"<item id=\"pubsub-item-1\" xmlns=\"http://jabber.org/protocol/pubsub\">"
						"<payload xmlns=\"tmp\"/>"
					"</item>"
					"<item id=\"pubsub-item-2\" xmlns=\"http://jabber.org/protocol/pubsub\">"
						"<payload xmlns=\"other-tmp\"/>"
					"</item>"
				"</items>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(items));
		}

		void testSerializeEmptyItems() {
			PubSubItemsSerializer serializer(&serializers);

			boost::shared_ptr<PubSubItems> items(boost::make_shared<PubSubItems>());

			std::string expectedResult = 
				"<items node=\"\" xmlns=\"http://jabber.org/protocol/pubsub\"/>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(items));
		}

	private:
		FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PubSubItemsSerializerTest);
