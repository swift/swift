/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/InBandRegistrationPayloadSerializer.h>

using namespace Swift;

class InBandRegistrationPayloadSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(InBandRegistrationPayloadSerializerTest);
        CPPUNIT_TEST(testSerialize);
        CPPUNIT_TEST(testSerialize_Form);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerialize() {
            InBandRegistrationPayloadSerializer testling;
            std::shared_ptr<InBandRegistrationPayload> registration(new InBandRegistrationPayload());
            registration->setRegistered(true);

            std::string expectedResult =
                "<query xmlns=\"jabber:iq:register\">"
                    "<registered/>"
                "</query>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(registration));
        }
        void testSerialize_Form() {
            InBandRegistrationPayloadSerializer testling;
            std::shared_ptr<InBandRegistrationPayload> registration(new InBandRegistrationPayload());
            registration->setInstructions("Use the enclosed form to register.");

            std::shared_ptr<Form> form(new Form(Form::FormType));
            form->setTitle("Contest Registration");

            FormField::ref field = std::make_shared<FormField>(FormField::HiddenType, "jabber:iq:register");
            field->setName("FORM_TYPE");
            form->addField(field);
            registration->setForm(form);

            std::string expectedResult =
                "<query xmlns=\"jabber:iq:register\">"
                    "<instructions>Use the enclosed form to register.</instructions>"
                    "<x type=\"form\" xmlns=\"jabber:x:data\">"
                        "<title>Contest Registration</title>"
                        "<field type=\"hidden\" var=\"FORM_TYPE\">"
                            "<value>jabber:iq:register</value>"
                        "</field>"
                    "</x>"
                "</query>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(registration));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(InBandRegistrationPayloadSerializerTest);
