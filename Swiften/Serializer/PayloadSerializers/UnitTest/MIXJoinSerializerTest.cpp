/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXJoinSerializer.h>

using namespace Swift;

TEST(MIXJoinSerializerTest, XEP0369_Example22) {
    MIXJoinSerializer testling;

    auto join = std::make_shared<MIXJoin>();
    join->setChannel(JID("coven@mix.shakespeare.example"));

    join->addSubscription(std::string("urn:xmpp:mix:nodes:messages"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:presence"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:participants"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:config"));

    std::string expectedResult = "<join channel=\"coven@mix.shakespeare.example\" xmlns=\"urn:xmpp:mix:0\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:config\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:participants\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                "</join>";
    ASSERT_EQ(expectedResult, testling.serialize(join));
}

TEST(MIXJoinSerializerTest, XEP0369_Example23) {
    MIXJoinSerializer testling;

    std::shared_ptr<MIXJoin> join(new MIXJoin());

    join->addSubscription(std::string("urn:xmpp:mix:nodes:messages"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:presence"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:participants"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:config"));

    std::string expectedResult = "<join xmlns=\"urn:xmpp:mix:0\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:config\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:participants\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                "</join>";
    ASSERT_EQ(expectedResult, testling.serialize(join));
}

TEST(MIXJoinSerializerTest, XEP0369_Example24) {
    MIXJoinSerializer testling;

    std::shared_ptr<MIXJoin> join(new MIXJoin());
    join->setJID(JID("123456#coven@mix.shakespeare.example"));

    join->addSubscription(std::string("urn:xmpp:mix:nodes:messages"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:presence"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:participants"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:config"));

    std::string expectedResult = "<join jid=\"123456#coven@mix.shakespeare.example\" xmlns=\"urn:xmpp:mix:0\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:config\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:participants\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                "</join>";
    ASSERT_EQ(expectedResult, testling.serialize(join));
}

TEST(MIXJoinSerializerTest, XEP0369_Example29) {
    MIXJoinSerializer testling;

    std::shared_ptr<MIXJoin> join(new MIXJoin());

    join->addSubscription(std::string("urn:xmpp:mix:nodes:messages"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:presence"));

    std::shared_ptr<Form> parameters(std::make_shared<Form>());
    parameters->setType(Form::Type::SubmitType);

    std::shared_ptr<FormField> fieldType = std::make_shared<FormField>(FormField::HiddenType);
    fieldType->setName("FORM_TYPE");
    fieldType->addValue("urn:xmpp:mix:0");
    parameters->addField(fieldType);

    std::shared_ptr<FormField> fieldJIDVisibility = std::make_shared<FormField>();
    fieldJIDVisibility->setName("JID Visibility");
    fieldJIDVisibility->addValue("never");
    parameters->addField(fieldJIDVisibility);

    join->setForm(parameters);

    std::string expectedResult = "<join xmlns=\"urn:xmpp:mix:0\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                    "<x type=\"submit\" xmlns=\"jabber:x:data\">"
                        "<field type=\"hidden\" var=\"FORM_TYPE\">"
                             "<value>urn:xmpp:mix:0</value>"
                        "</field>"
                        "<field var=\"JID Visibility\">"
                            "<value>never</value>"
                        "</field>"
                    "</x>"
                "</join>";
    ASSERT_EQ(expectedResult, testling.serialize(join));
}

TEST(MIXJoinSerializerTest, XEP0369_Example30) {
    MIXJoinSerializer testling;

    std::shared_ptr<MIXJoin> join(new MIXJoin());
    join->setJID(JID("hag66@shakespeare.example"));

    join->addSubscription(std::string("urn:xmpp:mix:nodes:messages"));
    join->addSubscription(std::string("urn:xmpp:mix:nodes:presence"));

    std::shared_ptr<Form> parameters(std::make_shared<Form>());
    parameters->setType(Form::Type::ResultType);

    std::shared_ptr<FormField> fieldType = std::make_shared<FormField>(FormField::HiddenType);
    fieldType->setName("FORM_TYPE");
    fieldType->addValue("urn:xmpp:mix:0");
    parameters->addField(fieldType);

    std::shared_ptr<FormField> fieldJIDVisibility = std::make_shared<FormField>();
    fieldJIDVisibility->setName("JID Visibility");
    fieldJIDVisibility->addValue("never");
    parameters->addField(fieldJIDVisibility);

    std::shared_ptr<FormField> fieldprivateMessages = std::make_shared<FormField>();
    fieldprivateMessages->setName("Private Messages");
    fieldprivateMessages->addValue("allow");
    parameters->addField(fieldprivateMessages);

    std::shared_ptr<FormField> vCard = std::make_shared<FormField>();
    vCard->setName("vCard");
    vCard->addValue("block");
    parameters->addField(vCard);

    join->setForm(parameters);

    std::string expectedResult = "<join jid=\"hag66@shakespeare.example\" xmlns=\"urn:xmpp:mix:0\">"
                    "<subscribe node=\"urn:xmpp:mix:nodes:messages\"/>"
                    "<subscribe node=\"urn:xmpp:mix:nodes:presence\"/>"
                    "<x type=\"result\" xmlns=\"jabber:x:data\">"
                        "<field type=\"hidden\" var=\"FORM_TYPE\">"
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
                "</join>";
    ASSERT_EQ(expectedResult, testling.serialize(join));
}
