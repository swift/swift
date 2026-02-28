/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/SoftwareVersionSerializer.h>

using namespace Swift;

class SoftwareVersionSerializerTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(SoftwareVersionSerializerTest);
        CPPUNIT_TEST(testSerialize);
        CPPUNIT_TEST_SUITE_END();

    public:
        SoftwareVersionSerializerTest() {}

        void testSerialize() {
            SoftwareVersionSerializer testling;
            std::shared_ptr<SoftwareVersion> softwareVersion(new SoftwareVersion("Swift", "0.1", "Mac OS X"));

            CPPUNIT_ASSERT_EQUAL(std::string("<query xmlns=\"jabber:iq:version\"><name>Swift</name><version>0.1</version><os>Mac OS X</os></query>"), testling.serialize(softwareVersion));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SoftwareVersionSerializerTest);
