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

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/UserTune.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/UserTuneSerializer.h>

using namespace Swift;

class UserTuneSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(UserTuneSerializerTest);
        CPPUNIT_TEST(testSerialize_withAllVariablesSet);
        CPPUNIT_TEST(testSerialize_withSomeVariablesSet);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerialize_withAllVariablesSet() {
            UserTuneSerializer testling(&serializers);
            std::shared_ptr<UserTune> userTune(new UserTune());
            userTune->setRating(5);
            userTune->setTitle(boost::optional<std::string>("Minion"));
            userTune->setTrack(boost::optional<std::string>("Yellow"));
            userTune->setArtist(boost::optional<std::string>("Ice"));
            userTune->setURI(boost::optional<std::string>("Fire"));
            userTune->setSource(boost::optional<std::string>("Origin"));
            userTune->setLength(226);

            std::string expectedResult =
                                "<tune xmlns=\"http://jabber.org/protocol/tune\">"
                                "<rating>5</rating><title>Minion</title><track>Yellow</track><artist>Ice</artist><uri>Fire</uri><source>Origin</source><length>226</length></tune>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(userTune));
        }

        void testSerialize_withSomeVariablesSet() {
            UserTuneSerializer testling(&serializers);
            std::shared_ptr<UserTune> userTune(new UserTune());
            userTune->setTitle(boost::optional<std::string>("Minion"));
            userTune->setTrack(boost::optional<std::string>("Yellow"));
            userTune->setArtist(boost::optional<std::string>("Ice"));
            userTune->setSource(boost::optional<std::string>("Origin"));
            userTune->setLength(226);

            std::string expectedResult =
                                "<tune xmlns=\"http://jabber.org/protocol/tune\">"
                                "<title>Minion</title><track>Yellow</track><artist>Ice</artist><source>Origin</source><length>226</length></tune>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(userTune));
        }
    private:
        FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(UserTuneSerializerTest);
