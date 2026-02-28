/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/ReferencePayloadSerializer.h>

#include <Swiften/Elements/Body.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

static FullPayloadSerializerCollection serializers;

TEST(ReferencePayloadSerializerTest, testSerialize) {
    ReferencePayloadSerializer testling(&serializers);
    auto reference = std::make_shared<ReferencePayload>();
    reference->setType(ReferencePayload::Type::Data);
    reference->setUri(boost::optional<std::string>("https://www.example.com/mindBlowingImage.jpeg"));
    reference->setBegin(boost::optional<std::string>("11"));
    reference->setEnd(boost::optional<std::string>("22"));
    reference->setAnchor(boost::optional<std::string>("xmpp:data@localhost.example.test"));

    std::string expectedResult =
        "<reference "
            "anchor=\"xmpp:data@localhost.example.test\" "
            "begin=\"11\" "
            "end=\"22\" "
            "type=\"data\" "
            "uri=\"https://www.example.com/mindBlowingImage.jpeg\" "
            "xmlns=\"urn:xmpp:reference:0\"/>";

    ASSERT_EQ(expectedResult, testling.serialize(reference));
}

TEST(ReferencePayloadSerializerTest, testSerializeNoType) {
    ReferencePayloadSerializer testling(&serializers);
    auto reference = std::make_shared<ReferencePayload>();
    reference->setUri(boost::optional<std::string>("https://www.example.com/mindBlowingImage.jpeg"));
    reference->setBegin(boost::optional<std::string>("11"));
    reference->setEnd(boost::optional<std::string>("22"));
    reference->setAnchor(boost::optional<std::string>("xmpp:data@localhost.example.test"));

    std::string expectedResult =
        "<reference "
            "anchor=\"xmpp:data@localhost.example.test\" "
            "begin=\"11\" "
            "end=\"22\" "
            "type=\"data\" "
            "uri=\"https://www.example.com/mindBlowingImage.jpeg\" "
            "xmlns=\"urn:xmpp:reference:0\"/>";

    ASSERT_EQ(expectedResult, testling.serialize(reference));
}

TEST(ReferencePayloadSerializerTest, testSerializeWithEmbeddedPayload) {
    ReferencePayloadSerializer testling(&serializers);
    auto reference = std::make_shared<ReferencePayload>();
    reference->setUri(boost::optional<std::string>("https://www.example.com/mindBlowingImage.jpeg"));
    reference->setBegin(boost::optional<std::string>("11"));
    reference->setEnd(boost::optional<std::string>("22"));
    reference->setAnchor(boost::optional<std::string>("xmpp:data@localhost.example.test"));
    auto payload = std::make_shared<Body>(std::string("Look, I'm in a reference"));
    reference->addPayload(payload);

    std::string expectedResult =
        "<reference "
            "anchor=\"xmpp:data@localhost.example.test\" "
            "begin=\"11\" "
            "end=\"22\" "
            "type=\"data\" "
            "uri=\"https://www.example.com/mindBlowingImage.jpeg\" "
            "xmlns=\"urn:xmpp:reference:0\">"
            "<body>Look, I'm in a reference</body>"
        "</reference>";

    ASSERT_EQ(expectedResult, testling.serialize(reference));
}
