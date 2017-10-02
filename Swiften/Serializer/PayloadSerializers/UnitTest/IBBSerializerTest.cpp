/*
 * Copyright (c) 2015 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/Serializer/PayloadSerializers/IBBSerializer.h>

using namespace Swift;

class IBBSerializerTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(IBBSerializerTest);
        CPPUNIT_TEST(testSerialize_data);
        CPPUNIT_TEST_SUITE_END();

    public:
        IBBSerializerTest() {}

        void testSerialize_data() {
            IBBSerializer testling;
            std::shared_ptr<IBB> ibb = std::make_shared<IBB>();
            ibb->setAction(IBB::Data);
            ibb->setData(createByteArray("abcdefgihjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890\x0a"));
            ibb->setSequenceNumber(4);
            CPPUNIT_ASSERT_EQUAL(std::string("<data seq=\"4\" sid=\"\" xmlns=\"http://jabber.org/protocol/ibb\">"
                    "YWJjZGVmZ2loamtsbW5vcHFyc3R1dnd4eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjEyMzQ1"
                    "Njc4OTAK"
                    "</data>"), testling.serialize(ibb));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(IBBSerializerTest);
