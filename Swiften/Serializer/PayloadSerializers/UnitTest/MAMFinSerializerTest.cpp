/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/MAMFinSerializer.h>

using namespace Swift;

class MAMFinSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(MAMFinSerializerTest);
        CPPUNIT_TEST(testSerialize_XEP0313_Exmaple1);
        CPPUNIT_TEST(testSerialize_XEP0313_Exmaple9);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerialize_XEP0313_Exmaple1() {
            MAMFinSerializer serializer;

            std::shared_ptr<MAMFin> fin = std::make_shared<MAMFin>();
            fin->setQueryID("f27");

            std::string expectedResult =
                "<fin queryid=\"f27\" xmlns=\"urn:xmpp:mam:0\"/>";
            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(fin));
        }

        void testSerialize_XEP0313_Exmaple9() {
            MAMFinSerializer serializer;

            std::shared_ptr<MAMFin> fin = std::make_shared<MAMFin>();
            fin->setComplete(true);

            std::shared_ptr<ResultSet> set = std::make_shared<ResultSet>();
            set->setFirstID(std::string("23452-4534-1"));
            set->setFirstIDIndex(0);
            set->setLastID(std::string("390-2342-22"));
            set->setCount(16);

            fin->setResultSet(set);

            std::string expectedResult =
                "<fin complete=\"true\" xmlns=\"urn:xmpp:mam:0\">"
                    "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                        "<count>16</count>"
                        "<first index=\"0\">23452-4534-1</first>"
                        "<last>390-2342-22</last>"
                    "</set>"
                "</fin>";
            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(fin));
        }

    private:
        FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MAMFinSerializerTest);
