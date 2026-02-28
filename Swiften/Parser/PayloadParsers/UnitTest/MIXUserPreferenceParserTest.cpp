/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXUserPreference.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXUserPreferenceParserTest, XEP0369_Example31) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<user-preference xmlns='urn:xmpp:mix:0'/>"
                ));

    auto payload = parser.getPayload<MIXUserPreference>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getData());
}

TEST(MIXUserPreferenceParserTest, XEP0369_Example32) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<user-preference xmlns='urn:xmpp:mix:0'>"
                    "<x xmlns='jabber:x:data' type='result'>"
                        "<field var='FORM_TYPE' type='hidden'>"
                            "<value>urn:xmpp:mix:0</value>"
                        "</field>"
                        "<field var='JID Visibility'>"
                            "<value>never</value>"
                        "</field>"
                        "<field var='Private Messages'>"
                           "<value>allow</value>"
                         "</field>"
                        "<field var='vCard'>"
                          "<value>block</value>"
                        "</field>"
                    "</x>"
                "</user-preference>"
                ));

    auto payload = parser.getPayload<MIXUserPreference>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getData());
    ASSERT_EQ(Form::Type::ResultType, payload->getData()->getType());
    std::shared_ptr<FormField> fieldType = payload->getData()->getField("FORM_TYPE");
    ASSERT_TRUE(fieldType);

    std::shared_ptr<FormField> fieldJIDVisibility = payload->getData()->getField("JID Visibility");
    ASSERT_TRUE(fieldJIDVisibility);
    ASSERT_EQ(std::string("never"), fieldJIDVisibility->getTextSingleValue());

    std::shared_ptr<FormField> fieldprivateMessages = payload->getData()->getField("Private Messages");
    ASSERT_TRUE(fieldprivateMessages);
    ASSERT_EQ(std::string("allow"), fieldprivateMessages->getTextSingleValue());

    std::shared_ptr<FormField> vCard = payload->getData()->getField("vCard");
    ASSERT_TRUE(vCard);
    ASSERT_EQ(std::string("block"), vCard->getTextSingleValue());
}
