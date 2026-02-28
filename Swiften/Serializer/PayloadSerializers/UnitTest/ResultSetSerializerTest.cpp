/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/ResultSet.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/ResultSetSerializer.h>

using namespace Swift;

class ResultSetSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ResultSetSerializerTest);
        CPPUNIT_TEST(testSerializeFull);
        CPPUNIT_TEST(testSerializeMaxItems);
        CPPUNIT_TEST(testSerializeEmptyBefore);
        CPPUNIT_TEST(testSerializeFirst);
        CPPUNIT_TEST(testSerializeFirstWithIndex);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerializeFull() {
            ResultSetSerializer serializer;

            std::shared_ptr<ResultSet> resultSet(std::make_shared<ResultSet>());

            resultSet->setMaxItems(100);
            resultSet->setCount(800);
            resultSet->setIndex(0);
            resultSet->setFirstIDIndex(123);
            resultSet->setFirstID(std::string("stpeter@jabber.org"));
            resultSet->setLastID(std::string("peterpan@neverland.lit"));
            resultSet->setAfter(std::string("09af3-cc343-b409f"));
            resultSet->setBefore(std::string("decaf-badba-dbad1"));

            std::string expectedResult =
                "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                    "<max>100</max>"
                    "<count>800</count>"
                    "<index>0</index>"
                    "<first index=\"123\">stpeter@jabber.org</first>"
                    "<last>peterpan@neverland.lit</last>"
                    "<before>decaf-badba-dbad1</before>"
                    "<after>09af3-cc343-b409f</after>"
                "</set>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(resultSet));
        }

        void testSerializeMaxItems() {
            ResultSetSerializer serializer;

            std::shared_ptr<ResultSet> resultSet(std::make_shared<ResultSet>());

            resultSet->setMaxItems(100);

            std::string expectedResult =
                "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                    "<max>100</max>"
                "</set>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(resultSet));
        }

        void testSerializeEmptyBefore() {
            ResultSetSerializer serializer;

            std::shared_ptr<ResultSet> resultSet(std::make_shared<ResultSet>());

            resultSet->setBefore(std::string());

            std::string expectedResult =
                "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                    "<before/>"
                "</set>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(resultSet));
        }

        void testSerializeFirst() {
            ResultSetSerializer serializer;

            std::shared_ptr<ResultSet> resultSet(std::make_shared<ResultSet>());

            resultSet->setFirstID(std::string("stpeter@jabber.org"));

            std::string expectedResult =
                "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                    "<first>stpeter@jabber.org</first>"
                "</set>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(resultSet));
        }

        void testSerializeFirstWithIndex() {
            ResultSetSerializer serializer;

            std::shared_ptr<ResultSet> resultSet(std::make_shared<ResultSet>());

            resultSet->setFirstID(std::string("stpeter@jabber.org"));
            resultSet->setFirstIDIndex(123);

            std::string expectedResult =
                "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                    "<first index=\"123\">stpeter@jabber.org</first>"
                "</set>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(resultSet));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ResultSetSerializerTest);
