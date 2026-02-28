/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/Form.h>

using namespace Swift;

class FormTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(FormTest);
        CPPUNIT_TEST(testGetFormType);
        CPPUNIT_TEST(testGetFormType_InvalidFormType);
        CPPUNIT_TEST(testGetFormType_NoFormType);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testGetFormType() {
            Form form;

            form.addField(std::make_shared<FormField>(FormField::FixedType, "Foo"));

            FormField::ref field = std::make_shared<FormField>(FormField::HiddenType, "jabber:bot");
            field->setName("FORM_TYPE");
            form.addField(field);

            form.addField(std::make_shared<FormField>(FormField::FixedType, "Bar"));

            CPPUNIT_ASSERT_EQUAL(std::string("jabber:bot"), form.getFormType());
        }

        void testGetFormType_InvalidFormType() {
            Form form;

            FormField::ref field = std::make_shared<FormField>(FormField::FixedType, "jabber:bot");
            field->setName("FORM_TYPE");
            form.addField(field);

            CPPUNIT_ASSERT_EQUAL(std::string(""), form.getFormType());
        }

        void testGetFormType_NoFormType() {
            Form form;

            form.addField(std::make_shared<FormField>(FormField::FixedType, "Foo"));

            CPPUNIT_ASSERT_EQUAL(std::string(""), form.getFormType());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(FormTest);
