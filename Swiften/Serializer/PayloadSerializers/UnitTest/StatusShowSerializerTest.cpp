/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/StatusShowSerializer.h>

using namespace Swift;

class StatusShowSerializerTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(StatusShowSerializerTest);
        CPPUNIT_TEST(testSerialize_Online);
        CPPUNIT_TEST(testSerialize_Away);
        CPPUNIT_TEST(testSerialize_FFC);
        CPPUNIT_TEST(testSerialize_XA);
        CPPUNIT_TEST(testSerialize_DND);
        CPPUNIT_TEST_SUITE_END();

    public:
        StatusShowSerializerTest() {}

        void testSerialize_Online() {
            StatusShowSerializer testling;
            std::shared_ptr<StatusShow> statusShow(new StatusShow(StatusShow::Online));
            CPPUNIT_ASSERT_EQUAL(std::string(""), testling.serialize(statusShow));
        }

        void testSerialize_Away() {
            StatusShowSerializer testling;
            std::shared_ptr<StatusShow> statusShow(new StatusShow(StatusShow::Away));
            CPPUNIT_ASSERT_EQUAL(std::string("<show>away</show>"), testling.serialize(statusShow));
        }

        void testSerialize_FFC() {
            StatusShowSerializer testling;
            std::shared_ptr<StatusShow> statusShow(new StatusShow(StatusShow::FFC));
            CPPUNIT_ASSERT_EQUAL(std::string("<show>chat</show>"), testling.serialize(statusShow));
        }

        void testSerialize_XA() {
            StatusShowSerializer testling;
            std::shared_ptr<StatusShow> statusShow(new StatusShow(StatusShow::XA));
            CPPUNIT_ASSERT_EQUAL(std::string("<show>xa</show>"), testling.serialize(statusShow));
        }

        void testSerialize_DND() {
            StatusShowSerializer testling;
            std::shared_ptr<StatusShow> statusShow(new StatusShow(StatusShow::DND));
            CPPUNIT_ASSERT_EQUAL(std::string("<show>dnd</show>"), testling.serialize(statusShow));
        }

        void testSerialize_None() {
            StatusShowSerializer testling;
            std::shared_ptr<StatusShow> statusShow(new StatusShow(StatusShow::None));
            CPPUNIT_ASSERT_EQUAL(std::string(""), testling.serialize(statusShow));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(StatusShowSerializerTest);
