/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXCreate.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXCreateParserTest, XEP0369_Example68) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<create xmlns=\"urn:xmpp:mix:0\"/>"
                ));

    auto payload = parser.getPayload<MIXCreate>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getData());
    ASSERT_FALSE(payload->getChannel());
}

TEST(MIXCreateParserTest, XEP0369_Example66) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<create channel=\"coven\" xmlns=\"urn:xmpp:mix:0\"/>"
                ));

    auto payload = parser.getPayload<MIXCreate>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getData());

    ASSERT_TRUE(payload->getChannel());
    ASSERT_EQ(std::string("coven"), *payload->getChannel());
}

TEST(MIXCreateParserTest, XEP0369_Example67) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                 "<create channel=\"coven\" xmlns=\"urn:xmpp:mix:0\">"
                    "<x xmlns=\"jabber:x:data\" type=\"result\">"
                        "<field var=\"FORM_TYPE\" type=\"hidden\">"
                            "<value>urn:xmpp:mix:0</value>"
                        "</field>"
                        "<field var=\"Owner\">"
                            "<value>hecate@shakespeare.lit</value>"
                            "<value>greymalkin@shakespeare.lit</value>"
                        "</field>"
                        "<field var=\"Messages Node Subscription\">"
                            "<value>allowed</value>"
                         "</field>"
                        "<field var=\"JID Visibility\">"
                            "<value>jid-mandatory-visible</value>"
                        "</field>"
                        "<field var=\"No Private Messages\">"
                            "<value>true</value>"
                        "</field>"
                    "</x>"
                "</create>"
                ));

    auto payload = parser.getPayload<MIXCreate>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getData());
    ASSERT_EQ(Form::Type::ResultType, payload->getData()->getType());
    std::shared_ptr<FormField> fieldType = payload->getData()->getField("FORM_TYPE");
    ASSERT_TRUE(fieldType);

    std::shared_ptr<FormField> fieldJIDVisibility = payload->getData()->getField("JID Visibility");
    ASSERT_TRUE(fieldJIDVisibility);
    ASSERT_EQ(std::string("jid-mandatory-visible"), fieldJIDVisibility->getTextSingleValue());

    std::shared_ptr<FormField> fieldprivateMessages = payload->getData()->getField("No Private Messages");
    ASSERT_TRUE(fieldprivateMessages);
    ASSERT_EQ(std::string("true"), fieldprivateMessages->getTextSingleValue());

    std::shared_ptr<FormField> nodeSubs = payload->getData()->getField("Messages Node Subscription");
    ASSERT_TRUE(nodeSubs);
    ASSERT_EQ(std::string("allowed"), nodeSubs->getTextSingleValue());

    ASSERT_TRUE(payload->getChannel());
    ASSERT_EQ(std::string("coven"), *payload->getChannel());
}
