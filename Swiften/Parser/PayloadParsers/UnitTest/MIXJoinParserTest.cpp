/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXJoinParserTest, XEP0369_Example22) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<join xmlns=\"urn:xmpp:mix:0\" channel=\"coven@mix.shakespeare.example\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:participants\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:config\"/>"
                "</join>"
                ));

    MIXJoin::ref payload = parser.getPayload<MIXJoin>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getChannel());
    ASSERT_EQ(JID("coven@mix.shakespeare.example"), *payload->getChannel());
    ASSERT_FALSE(payload->getJID());
    ASSERT_FALSE(payload->getForm());

    ASSERT_EQ(static_cast<size_t>(4), payload->getSubscriptions().size());
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:messages")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:presence")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:participants")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:config")));
}

TEST(MIXJoinParserTest, XEP0369_Example23) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<join xmlns=\"urn:xmpp:mix:0\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:participants\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:config\"/>"
                "</join>"
                ));

    MIXJoin::ref payload = parser.getPayload<MIXJoin>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getChannel());
    ASSERT_FALSE(payload->getJID());
    ASSERT_FALSE(payload->getForm());

    ASSERT_EQ(static_cast<size_t>(4), payload->getSubscriptions().size());
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:messages")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:presence")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:participants")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:config")));
}

TEST(MIXJoinParserTest, XEP0369_Example24) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<join xmlns=\"urn:xmpp:mix:0\" jid=\"123456#coven@mix.shakespeare.example\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:participants\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:config\"/>"
                "</join>"
                ));

    MIXJoin::ref payload = parser.getPayload<MIXJoin>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getChannel());
    ASSERT_TRUE(payload->getJID());
    ASSERT_EQ(JID("123456#coven@mix.shakespeare.example"), *payload->getJID());
    ASSERT_FALSE(payload->getForm());

    ASSERT_EQ(static_cast<size_t>(4), payload->getSubscriptions().size());
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:messages")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:presence")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:participants")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:config")));
}

TEST(MIXJoinParserTest, XEP0369_Example29) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<join xmlns=\"urn:xmpp:mix:0\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                    "<x xmlns=\"jabber:x:data\" type=\"submit\">"
                        "<field var=\"FORM_TYPE\" type=\"hidden\">"
                             "<value>urn:xmpp:mix:0</value>"
                        "</field>"
                        "<field var=\"JID Visibility\">"
                            "<value>never</value>"
                        "</field>"
                    "</x>"
                "</join>"));

    MIXJoin::ref payload = parser.getPayload<MIXJoin>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getChannel());
    ASSERT_FALSE(payload->getJID());

    ASSERT_EQ(static_cast<size_t>(2), payload->getSubscriptions().size());
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:messages")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:presence")));

    ASSERT_TRUE(payload->getForm());
    ASSERT_EQ(Form::Type::SubmitType, payload->getForm()->getType());
    std::shared_ptr<FormField> fieldType = payload->getForm()->getField("FORM_TYPE");
    ASSERT_TRUE(fieldType);

    std::shared_ptr<FormField> fieldJIDVisibility = payload->getForm()->getField("JID Visibility");
    ASSERT_TRUE(fieldJIDVisibility);
    ASSERT_EQ(std::string("never"), fieldJIDVisibility->getTextSingleValue());
}

TEST(MIXJoinParserTest, XEP0369_Example30) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
        "<join xmlns=\"urn:xmpp:mix:0\" jid=\"hag66@shakespeare.example\">"
            "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
            "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
            "<x xmlns=\"jabber:x:data\" type=\"result\">"
                "<field var=\"FORM_TYPE\" type=\"hidden\">"
                    "<value>urn:xmpp:mix:0</value>"
                "</field>"
                "<field var=\"JID Visibility\">"
                    "<value>never</value>"
                "</field>"
                "<field var=\"Private Messages\">"
                    "<value>allow</value>"
                "</field>"
                "<field var=\"vCard\">"
                    "<value>block</value>"
                "</field>"
            "</x>"
        "</join>"));

    MIXJoin::ref payload = parser.getPayload<MIXJoin>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getChannel());
    ASSERT_TRUE(payload->getJID());
    ASSERT_EQ(JID("hag66@shakespeare.example"), *payload->getJID());

    ASSERT_EQ(static_cast<size_t>(2), payload->getSubscriptions().size());
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:messages")));
    ASSERT_TRUE(payload->hasSubscription(std::string("urn:xmpp:mix:nodes:presence")));

    ASSERT_TRUE(payload->getForm());
    ASSERT_EQ(Form::Type::ResultType, payload->getForm()->getType());
    std::shared_ptr<FormField> fieldType = payload->getForm()->getField("FORM_TYPE");
    ASSERT_TRUE(fieldType);

    std::shared_ptr<FormField> fieldJIDVisibility = payload->getForm()->getField("JID Visibility");
    ASSERT_TRUE(fieldJIDVisibility);
    ASSERT_EQ(std::string("never"), fieldJIDVisibility->getTextSingleValue());

    std::shared_ptr<FormField> fieldprivateMessages = payload->getForm()->getField("Private Messages");
    ASSERT_TRUE(fieldprivateMessages);
    ASSERT_EQ(std::string("allow"), fieldprivateMessages->getTextSingleValue());

    std::shared_ptr<FormField> vCard = payload->getForm()->getField("vCard");
    ASSERT_TRUE(vCard);
    ASSERT_EQ(std::string("block"), vCard->getTextSingleValue());
}
