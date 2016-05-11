/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubRetractSerializer.h>

using namespace Swift;

class PubSubRetractSerializerTest: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PubSubRetractSerializerTest);
    CPPUNIT_TEST(testSerializeDefault);
    CPPUNIT_TEST(testSerializeNotify);
    CPPUNIT_TEST_SUITE_END();

    public:
        PubSubRetractSerializerTest() {}

        std::shared_ptr<PubSubItem> somePubSubItem() {
            auto item = std::make_shared<PubSubItem>();
            item->setID("ae890ac52d0df67ed7cfdf51b644e901");
            return item;
        }

        void testSerializeDefault() {
            PubSubRetractSerializer testling(&serializer);
            auto retract = std::make_shared<PubSubRetract>();
            retract->setNode("princely_musings");
            retract->setItems({somePubSubItem()});
            CPPUNIT_ASSERT_EQUAL(std::string("<retract node=\"princely_musings\" xmlns=\"http://jabber.org/protocol/pubsub\"><item id=\"ae890ac52d0df67ed7cfdf51b644e901\" xmlns=\"http://jabber.org/protocol/pubsub\"/></retract>"), testling.serialize(retract));
        }

        void testSerializeNotify() {
            PubSubRetractSerializer testling(&serializer);
            auto retract = std::make_shared<PubSubRetract>();
            retract->setNode("princely_musings");
            retract->setItems({somePubSubItem()});
            retract->setNotify(true);
            CPPUNIT_ASSERT_EQUAL(std::string("<retract node=\"princely_musings\" notify=\"true\" xmlns=\"http://jabber.org/protocol/pubsub\"><item id=\"ae890ac52d0df67ed7cfdf51b644e901\" xmlns=\"http://jabber.org/protocol/pubsub\"/></retract>"), testling.serialize(retract));

            retract = std::make_shared<PubSubRetract>();
            retract->setNode("princely_musings");
            retract->setItems({somePubSubItem()});
            retract->setNotify(false);
            CPPUNIT_ASSERT_EQUAL(std::string("<retract node=\"princely_musings\" notify=\"false\" xmlns=\"http://jabber.org/protocol/pubsub\"><item id=\"ae890ac52d0df67ed7cfdf51b644e901\" xmlns=\"http://jabber.org/protocol/pubsub\"/></retract>"), testling.serialize(retract));
        }

    private:
        FullPayloadSerializerCollection serializer;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PubSubRetractSerializerTest);
