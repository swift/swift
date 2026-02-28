/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/Body.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Parser/PayloadParsers/ReferencePayloadParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(ReferencePayloadParserTest, testParse) {
    PayloadsParserTester parser;

    ASSERT_TRUE(parser.parse(
        "<reference xmlns='urn:xmpp:reference:0' "
            "type='data' "
            "uri='https://www.example.com/mindBlowingImage.jpeg' "
            "begin='11' "
            "end='22' "
            "anchor='xmpp:data@localhost.example.test'>"
        "</reference>"));

    auto payload = std::dynamic_pointer_cast<ReferencePayload>(parser.getPayload());
    ASSERT_EQ(static_cast<int>(ReferencePayload::Type::Data), static_cast<int>(payload->getType()));
    ASSERT_EQ(std::string("https://www.example.com/mindBlowingImage.jpeg"), *payload->getUri());
    ASSERT_EQ(std::string("11"), *payload->getBegin());
    ASSERT_EQ(std::string("22"), *payload->getEnd());
    ASSERT_EQ(std::string("xmpp:data@localhost.example.test"), *payload->getAnchor());
}

TEST(ReferencePayloadParserTest, testParseNoType) {
    PayloadsParserTester parser;

    ASSERT_TRUE(parser.parse(
        "<reference xmlns='urn:xmpp:reference:0' "
            "uri='https://www.example.com/mindBlowingImage.jpeg' "
            "begin='11' "
            "end='22' "
            "anchor='xmpp:data@localhost.example.test'>"
        "</reference>"));

    auto payload = std::dynamic_pointer_cast<ReferencePayload>(parser.getPayload());
    ASSERT_EQ(static_cast<int>(ReferencePayload::Type::Unknown), static_cast<int>(payload->getType()));
    ASSERT_EQ(std::string("https://www.example.com/mindBlowingImage.jpeg"), *payload->getUri());
    ASSERT_EQ(std::string("11"), *payload->getBegin());
    ASSERT_EQ(std::string("22"), *payload->getEnd());
    ASSERT_EQ(std::string("xmpp:data@localhost.example.test"), *payload->getAnchor());
}

TEST(ReferencePayloadParserTest, testParseEmbeddedPayloads) {
    PayloadsParserTester parser;

    ASSERT_TRUE(parser.parse(
        "<reference xmlns='urn:xmpp:reference:0' type='data'> "
            "<error type=\"modify\">"
                "<bad-request xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/>"
                "<delay xmlns='urn:xmpp:delay' from='juliet@capulet.com/balcony' stamp='2002-09-10T23:41:07Z'/>"
                "<text xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\">boo</text>"
            "</error>"
        "</reference>"));

    auto payload = std::dynamic_pointer_cast<ReferencePayload>(parser.getPayload());
    ASSERT_EQ(static_cast<int>(ReferencePayload::Type::Data), static_cast<int>(payload->getType()));
    ASSERT_FALSE(payload->getUri());
    ASSERT_FALSE(payload->getBegin());
    ASSERT_FALSE(payload->getEnd());
    ASSERT_FALSE(payload->getAnchor());
    auto childPayloadList = payload->getPayloads();
    auto errorPayload = std::dynamic_pointer_cast<ErrorPayload>(childPayloadList[0]);
    ASSERT_TRUE(errorPayload);
    ASSERT_EQ("boo", errorPayload->getText());
    auto delayPayload = std::dynamic_pointer_cast<Delay>(errorPayload->getPayload());
    ASSERT_TRUE(delayPayload);
}

TEST(ReferencePayloadParserTest, testParseEmbeddedPayloadWithText) {
    PayloadsParserTester parser;

    ASSERT_TRUE(parser.parse(
        "<reference xmlns='urn:xmpp:reference:0' type='data'> "
            "<body>Example Text</body>"
        "</reference>"));

    auto payload = std::dynamic_pointer_cast<ReferencePayload>(parser.getPayload());
    auto childPayloadList = payload->getPayloads();
    auto bodyPayload = std::dynamic_pointer_cast<Body>(childPayloadList[0]);
    ASSERT_EQ("Example Text", bodyPayload->getText());
}

TEST(ReferencePayloadParserTest, testParseRecursive) {
    PayloadsParserTester parser;

    ASSERT_TRUE(parser.parse(
        "<reference xmlns='urn:xmpp:reference:0' type='data'> "
            "<reference xmlns='urn:xmpp:reference:0' type='data' uri='https://download.montague.lit/4a771ac1-f0b2-4a4a-9700-f2a26fa2bb67/summit.jpg' /> "
            "<reference xmlns='urn:xmpp:reference:0' type='data' uri='xmpp:romeo@montague.lit/resource?jingle;id=9559976B-3FBF-4E7E-B457-2DAA225972BB' /> "
            "<reference xmlns='urn:xmpp:reference:0' type='data'> "
                "<reference xmlns='urn:xmpp:reference:0' type='data' uri='https://www.example.com/mindBlowingImage.jpeg' /> "
            "</reference>"
        "</reference>"));

    auto payload = std::dynamic_pointer_cast<ReferencePayload>(parser.getPayload());
    ASSERT_EQ(static_cast<int>(ReferencePayload::Type::Data), static_cast<int>(payload->getType()));
    auto childPayloadList = payload->getPayloads();
    auto childPayloadA = std::dynamic_pointer_cast<ReferencePayload>(childPayloadList[0]);
    auto childPayloadB = std::dynamic_pointer_cast<ReferencePayload>(childPayloadList[1]);
    auto childPayloadC = std::dynamic_pointer_cast<ReferencePayload>(childPayloadList[2]);
    ASSERT_TRUE(childPayloadA);
    ASSERT_TRUE(childPayloadB);
    ASSERT_TRUE(childPayloadC);
    ASSERT_EQ(static_cast<int>(ReferencePayload::Type::Data), static_cast<int>(childPayloadA->getType()));
    ASSERT_EQ(static_cast<int>(ReferencePayload::Type::Data), static_cast<int>(childPayloadB->getType()));
    ASSERT_EQ(static_cast<int>(ReferencePayload::Type::Data), static_cast<int>(childPayloadC->getType()));
    ASSERT_EQ(std::string("https://download.montague.lit/4a771ac1-f0b2-4a4a-9700-f2a26fa2bb67/summit.jpg"), *childPayloadA->getUri());
    ASSERT_EQ(std::string("xmpp:romeo@montague.lit/resource?jingle;id=9559976B-3FBF-4E7E-B457-2DAA225972BB"), *childPayloadB->getUri());
    ASSERT_FALSE(childPayloadC->getUri());
    ASSERT_FALSE(childPayloadC->getBegin());
    ASSERT_FALSE(childPayloadC->getEnd());
    ASSERT_FALSE(childPayloadC->getAnchor());
    auto grandChildPayloadList = childPayloadC->getPayloads();
    auto grandChildPayload = std::dynamic_pointer_cast<ReferencePayload>(grandChildPayloadList[0]);
    ASSERT_TRUE(grandChildPayload);
    ASSERT_EQ(static_cast<int>(ReferencePayload::Type::Data), static_cast<int>(grandChildPayload->getType()));
    ASSERT_EQ(std::string("https://www.example.com/mindBlowingImage.jpeg"), *grandChildPayload->getUri());
    ASSERT_FALSE(grandChildPayload->getBegin());
    ASSERT_FALSE(grandChildPayload->getEnd());
    ASSERT_FALSE(grandChildPayload->getAnchor());
}
