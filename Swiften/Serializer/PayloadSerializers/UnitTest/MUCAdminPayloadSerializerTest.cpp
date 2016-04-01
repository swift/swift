/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/MUCAdminPayloadSerializer.h>

using namespace Swift;

class MUCAdminPayloadSerializerTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(MUCAdminPayloadSerializerTest);
        CPPUNIT_TEST(testSerialize);
        CPPUNIT_TEST_SUITE_END();

    public:
        MUCAdminPayloadSerializerTest() {}

        void testSerialize() {
            MUCAdminPayloadSerializer testling;
            std::shared_ptr<MUCAdminPayload> admin = std::make_shared<MUCAdminPayload>();
            MUCItem item;
            item.affiliation = MUCOccupant::Owner;
            item.role = MUCOccupant::Visitor;
            item.reason = "malice";
            item.actor = JID("kev@tester.lit");
            admin->addItem(item);

            CPPUNIT_ASSERT_EQUAL(std::string("<query xmlns=\"http://jabber.org/protocol/muc#admin\"><item affiliation=\"owner\" role=\"visitor\"><actor jid=\"kev@tester.lit\"/><reason>malice</reason></item></query>"), testling.serialize(admin));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCAdminPayloadSerializerTest);
