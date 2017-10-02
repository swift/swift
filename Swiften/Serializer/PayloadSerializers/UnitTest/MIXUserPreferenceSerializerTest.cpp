/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXUserPreferenceSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

TEST(MIXUserPreferenceSerializerTest, XEP0369_Example31) {
    MIXUserPreferenceSerializer testling;

    auto userpreference = std::shared_ptr<MIXUserPreference>(new MIXUserPreference());
    std::string expectedResult = "<user-preference xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(userpreference));
}

TEST(MIXUserPreferenceSerializerTest, XEP0369_Example32) {
    MIXUserPreferenceSerializer testling;

    auto userpreference = std::shared_ptr<MIXUserPreference>(new MIXUserPreference());
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

    userpreference->setData(parameters);

    std::string expectedResult = "<user-preference xmlns=\"urn:xmpp:mix:0\">"
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
                                "</user-preference>";
    ASSERT_EQ(expectedResult, testling.serialize(userpreference));
}
