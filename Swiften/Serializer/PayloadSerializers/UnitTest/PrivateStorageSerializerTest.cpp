/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/PrivateStorage.h>
#include <Swiften/Elements/Storage.h>
#include <Swiften/Serializer/PayloadSerializers/UnitTest/PayloadsSerializer.h>

using namespace Swift;

class PrivateStorageSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(PrivateStorageSerializerTest);
        CPPUNIT_TEST(testSerialize);
        CPPUNIT_TEST_SUITE_END();

    public:
        PrivateStorageSerializerTest() {}

        void testSerialize() {
            PayloadsSerializer serializer;

            std::shared_ptr<PrivateStorage> privateStorage(new PrivateStorage());
            std::shared_ptr<Storage> storage(new Storage());
            Storage::Room room;
            room.name = "Swift";
            room.jid = JID("swift@rooms.swift.im");
            room.nick = "Alice";
            storage->addRoom(room);
            privateStorage->setPayload(storage);

            CPPUNIT_ASSERT_EQUAL(std::string(
                "<query xmlns=\"jabber:iq:private\">"
                    "<storage xmlns=\"storage:bookmarks\">"
                        "<conference "
                                "autojoin=\"0\" "
                                "jid=\"swift@rooms.swift.im\" "
                                "name=\"Swift\">"
                            "<nick>Alice</nick>"
                        "</conference>"
                    "</storage>"
                "</query>"), serializer.serialize(privateStorage));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PrivateStorageSerializerTest);
