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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/InBandRegistrationPayload.h>
#include <Swiften/Parser/PayloadParsers/InBandRegistrationPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class InBandRegistrationPayloadParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(InBandRegistrationPayloadParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST(testParse_Form);
        CPPUNIT_TEST_SUITE_END();

    public:
        InBandRegistrationPayloadParserTest() {}

        void testParse() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns=\"jabber:iq:register\">"
                                "<registered/>"
                                "</query>"));

            InBandRegistrationPayload* payload = dynamic_cast<InBandRegistrationPayload*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT(payload->isRegistered());
        }

        void testParse_Form() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                                    "<query xmlns=\"jabber:iq:register\">"
                                    "<instructions>Use the enclosed form to register.</instructions>"
                                    "<x type=\"form\" xmlns=\"jabber:x:data\">"
                                    "<title>Contest Registration</title>"
                                    "<field type=\"hidden\" var=\"FORM_TYPE\">"
                                    "<value>jabber:iq:register</value>"
                                    "</field>"
                                    "</x>"
                                    "</query>"));

            InBandRegistrationPayload* payload = dynamic_cast<InBandRegistrationPayload*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            boost::optional<std::string> instruction = payload->getInstructions();
            CPPUNIT_ASSERT(instruction);
            CPPUNIT_ASSERT_EQUAL(std::string("Use the enclosed form to register."), instruction.get());

            Form::ref form = payload->getForm();
            CPPUNIT_ASSERT(form);
            CPPUNIT_ASSERT_EQUAL(std::string("Contest Registration"), form->getTitle());
            CPPUNIT_ASSERT_EQUAL(Form::FormType, form->getType());
            CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:register"), form->getFormType());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(InBandRegistrationPayloadParserTest);
