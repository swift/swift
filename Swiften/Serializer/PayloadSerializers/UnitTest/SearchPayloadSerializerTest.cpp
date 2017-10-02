/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/SearchPayloadSerializer.h>

using namespace Swift;

class SearchPayloadSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(SearchPayloadSerializerTest);
        CPPUNIT_TEST(testSerialize_Request);
        CPPUNIT_TEST(testSerialize_Items);
        CPPUNIT_TEST(testSerialize_DataForm);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerialize_Request() {
            SearchPayloadSerializer testling;

            SearchPayload::ref payload(new SearchPayload());
            payload->setFirst("Juliet");
            payload->setLast("Capulet");

            CPPUNIT_ASSERT_EQUAL(std::string(
                    "<query xmlns=\"jabber:iq:search\">"
                        "<first>Juliet</first>"
                        "<last>Capulet</last>"
                    "</query>"
                ), testling.serialize(payload));
        }

        void testSerialize_Items() {
            SearchPayloadSerializer testling;

            SearchPayload::ref payload(new SearchPayload());
            SearchPayload::Item item1;
            item1.jid = JID("juliet@capulet.com");
            item1.first = "Juliet";
            item1.last = "Capulet";
            item1.nick = "JuliC";
            item1.email = "juliet@shakespeare.lit";
            payload->addItem(item1);

            SearchPayload::Item item2;
            item2.jid = JID("tybalt@shakespeare.lit");
            item2.first = "Tybalt";
            item2.last = "Capulet";
            item2.nick = "ty";
            item2.email = "tybalt@shakespeare.lit";
            payload->addItem(item2);

            CPPUNIT_ASSERT_EQUAL(std::string(
                    "<query xmlns=\"jabber:iq:search\">"
                        "<item jid=\"juliet@capulet.com\">"
                            "<first>Juliet</first>"
                            "<last>Capulet</last>"
                            "<nick>JuliC</nick>"
                            "<email>juliet@shakespeare.lit</email>"
                        "</item>"
                        "<item jid=\"tybalt@shakespeare.lit\">"
                            "<first>Tybalt</first>"
                            "<last>Capulet</last>"
                            "<nick>ty</nick>"
                            "<email>tybalt@shakespeare.lit</email>"
                        "</item>"
                    "</query>"
                ), testling.serialize(payload));
        }

        void testSerialize_DataForm() {
            SearchPayloadSerializer testling;

            SearchPayload::ref payload(new SearchPayload());
            std::shared_ptr<Form> form(new Form(Form::ResultType));

            FormField::ref field = std::make_shared<FormField>(FormField::HiddenType, "jabber:iq:search");
            field->setName("FORM_TYPE");
            form->addField(field);

            // reported fields
            field = std::make_shared<FormField>(FormField::TextSingleType);
            field->setName("first");
            field->setLabel("Given Name");
            form->addReportedField(field);

            field = std::make_shared<FormField>(FormField::TextSingleType);
            field->setName("last");
            field->setLabel("Family Name");
            form->addReportedField(field);

            field = std::make_shared<FormField>(FormField::JIDSingleType);
            field->setName("jid");
            field->setLabel("Jabber ID");
            form->addReportedField(field);

            field = std::make_shared<FormField>(FormField::ListSingleType);
            field->setName("x-gender");
            field->setLabel("Gender");
            form->addReportedField(field);

            Form::FormItem firstItem;
            field = std::make_shared<FormField>(FormField::TextSingleType, "Benvolio");
            field->setName("first");
            firstItem.push_back(field);

            field = std::make_shared<FormField>(FormField::TextSingleType, "Montague");
            field->setName("last");
            firstItem.push_back(field);

            field = std::make_shared<FormField>(FormField::TextSingleType, "benvolio@montague.net");
            field->setName("jid");
            firstItem.push_back(field);

            field = std::make_shared<FormField>(FormField::ListSingleType, "male");
            field->setName("x-gender");
            firstItem.push_back(field);

            Form::FormItem secondItem;
            field = std::make_shared<FormField>(FormField::TextSingleType, "Romeo");
            field->setName("first");
            secondItem.push_back(field);

            field = std::make_shared<FormField>(FormField::TextSingleType, "Montague");
            field->setName("last");
            secondItem.push_back(field);

            field = std::make_shared<FormField>(FormField::TextSingleType, "romeo@montague.net");
            field->setName("jid");
            secondItem.push_back(field);

            field = std::make_shared<FormField>(FormField::ListSingleType, "male");
            field->setName("x-gender");
            secondItem.push_back(field);

            form->addItem(firstItem);
            form->addItem(secondItem);

            payload->setForm(form);

            CPPUNIT_ASSERT_EQUAL(std::string(
                "<query xmlns=\"jabber:iq:search\">"
                    "<x type=\"result\" xmlns=\"jabber:x:data\">"
                        "<field type=\"hidden\" var=\"FORM_TYPE\">"
                            "<value>jabber:iq:search</value>"
                        "</field>"
                        "<reported>"
                            "<field label=\"Given Name\" type=\"text-single\" var=\"first\"/>"
                            "<field label=\"Family Name\" type=\"text-single\" var=\"last\"/>"
                            "<field label=\"Jabber ID\" type=\"jid-single\" var=\"jid\"/>"
                            "<field label=\"Gender\" type=\"list-single\" var=\"x-gender\"/>"
                        "</reported>"
                        "<item>"
                            "<field var=\"first\"><value>Benvolio</value></field>"
                            "<field var=\"last\"><value>Montague</value></field>"
                            "<field var=\"jid\"><value>benvolio@montague.net</value></field>"
                            "<field var=\"x-gender\"><value>male</value></field>"
                        "</item>"
                        "<item>"
                            "<field var=\"first\"><value>Romeo</value></field>"
                            "<field var=\"last\"><value>Montague</value></field>"
                            "<field var=\"jid\"><value>romeo@montague.net</value></field>"
                            "<field var=\"x-gender\"><value>male</value></field>"
                        "</item>"
                    "</x>"
                "</query>"), testling.serialize(payload));

        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SearchPayloadSerializerTest);
