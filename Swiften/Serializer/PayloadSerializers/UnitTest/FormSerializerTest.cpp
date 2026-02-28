/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
using namespace Swift;

class FormSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(FormSerializerTest);
        CPPUNIT_TEST(testSerializeFormInformation);
        CPPUNIT_TEST(testSerializeLayout);
        CPPUNIT_TEST(testSerializeFields);
        CPPUNIT_TEST(testSerializeFormItems);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerializeFormInformation() {
            FormSerializer testling;
            std::shared_ptr<Form> form(new Form(Form::FormType));
            form->setTitle("Bot Configuration");
            form->setInstructions("Hello!\nFill out this form to configure your new bot!");

            CPPUNIT_ASSERT_EQUAL(std::string(
                "<x type=\"form\" xmlns=\"jabber:x:data\">"
                        "<title>Bot Configuration</title>"
                        "<instructions>Hello!</instructions>"
                        "<instructions>Fill out this form to configure your new bot!</instructions>"
                    "</x>"), testling.serialize(form));
        }

        void testSerializeLayout() {
            FormSerializer testling;
            std::shared_ptr<Form> form(new Form(Form::FormType));

            FormPage::page page = std::make_shared<FormPage>();
            page->setLabel("P1");
            FormReportedRef::ref reportedRef = std::make_shared<FormReportedRef>();
            page->addReportedRef(reportedRef);
            FormText::text formText = std::make_shared<FormText>();
            formText->setTextString("P1T1");
            page->addTextElement(formText);
            FormField::ref field = std::make_shared<FormField>(FormField::TextSingleType);
            field->setName("P1F1");
            field->setLabel("field one");
            page->addField(field);

            FormSection::section section = std::make_shared<FormSection>();
            section->setLabel("P1S1");
            formText = std::make_shared<FormText>();
            formText->setTextString("P1S1T1");
            section->addTextElement(formText);
            field = std::make_shared<FormField>(FormField::TextSingleType);
            field->setName("P1S1F1");
            field->setLabel("field two");
            section->addField(field);
            page->addChildSection(section);
            form->addPage(page);

            page = std::make_shared<FormPage>();
            page->setLabel("P2");
            section = std::make_shared<FormSection>();
            section->setLabel("P2S1");
            FormSection::section subSection = std::make_shared<FormSection>();
            subSection->setLabel("P2S2");
            FormSection::section subSection2 = std::make_shared<FormSection>();
            subSection2->setLabel("P2S3");
            subSection->addChildSection(subSection2);
            section->addChildSection(subSection);
            page->addChildSection(section);
            form->addPage(page);

            // P1 = page one, S1 = section one, F1 = field one, T1 = text one
            CPPUNIT_ASSERT_EQUAL(std::string(
                "<x type=\"form\" xmlns=\"jabber:x:data\">"
                    "<page label=\"P1\" xmlns=\"http://jabber.org/protocol/xdata-layout\">"
                        "<text>P1T1</text>"
                        "<fieldref var=\"P1F1\"/>"
                        "<reportedref/>"
                        "<section label=\"P1S1\">"
                            "<text>P1S1T1</text>"
                            "<fieldref var=\"P1S1F1\"/>"
                        "</section>"
                    "</page>"
                    "<page label=\"P2\" xmlns=\"http://jabber.org/protocol/xdata-layout\">"
                        "<section label=\"P2S1\">"
                            "<section label=\"P2S2\">"
                                "<section label=\"P2S3\"/>"
                            "</section>"
                        "</section>"
                    "</page>"
                    "<field label=\"field one\" type=\"text-single\" var=\"P1F1\"/>"
                    "<field label=\"field two\" type=\"text-single\" var=\"P1S1F1\"/>"
                "</x>"), testling.serialize(form));
        }

        void testSerializeFields() {
            FormSerializer testling;
            std::shared_ptr<Form> form(new Form(Form::FormType));

            FormField::ref field = std::make_shared<FormField>(FormField::HiddenType, "jabber:bot");
            field->setName("FORM_TYPE");
            form->addField(field);

            form->addField(std::make_shared<FormField>(FormField::FixedType, "Section 1: Bot Info"));

            field = std::make_shared<FormField>(FormField::TextSingleType);
            field->setName("botname");
            field->setLabel("The name of your bot");
            form->addField(field);

            field = std::make_shared<FormField>(FormField::TextMultiType);
            field->setTextMultiValue("This is a bot.\nA quite good one actually");
            field->setName("description");
            field->setLabel("Helpful description of your bot");
            form->addField(field);

            field = std::make_shared<FormField>(FormField::BooleanType, "1");
            field->setName("public");
            field->setLabel("Public bot?");
            field->setRequired(true);
            form->addField(field);

            field = std::make_shared<FormField>(FormField::TextPrivateType);
            field->setName("password");
            field->setLabel("Password for special access");
            form->addField(field);

            field = std::make_shared<FormField>(FormField::ListMultiType);
            field->addValue("news");
            field->addValue("search");
            field->setName("features");
            field->setLabel("What features will the bot support?");
            field->addOption(FormField::Option("Contests", "contests"));
            field->addOption(FormField::Option("News", "news"));
            field->addOption(FormField::Option("Polls", "polls"));
            field->addOption(FormField::Option("Reminders", "reminders"));
            field->addOption(FormField::Option("Search", "search"));
            form->addField(field);

            field = std::make_shared<FormField>(FormField::ListSingleType, "20");
            field->setName("maxsubs");
            field->setLabel("Maximum number of subscribers");
            field->addOption(FormField::Option("10", "10"));
            field->addOption(FormField::Option("20", "20"));
            field->addOption(FormField::Option("30", "30"));
            field->addOption(FormField::Option("50", "50"));
            field->addOption(FormField::Option("100", "100"));
            field->addOption(FormField::Option("", "none"));
            form->addField(field);

            std::vector<JID> jids;
            field = std::make_shared<FormField>(FormField::JIDMultiType);
            field->addValue("foo@bar.com");
            field->addValue("baz@fum.org");
            field->setName("invitelist");
            field->setLabel("People to invite");
            field->setDescription("Tell all your friends about your new bot!");
            form->addField(field);

            CPPUNIT_ASSERT_EQUAL(std::string(
                "<x type=\"form\" xmlns=\"jabber:x:data\">"
                        "<field type=\"hidden\" var=\"FORM_TYPE\">"
                            "<value>jabber:bot</value>"
                        "</field>"
                        "<field type=\"fixed\"><value>Section 1: Bot Info</value></field>"
                        "<field label=\"The name of your bot\" type=\"text-single\" var=\"botname\"/>"
                        "<field label=\"Helpful description of your bot\" type=\"text-multi\" var=\"description\"><value>This is a bot.</value><value>A quite good one actually</value></field>"
                        "<field label=\"Public bot?\" type=\"boolean\" var=\"public\">"
                            "<required/>"
                            "<value>1</value>"
                        "</field>"
                        "<field label=\"Password for special access\" type=\"text-private\" var=\"password\"/>"
                        "<field label=\"What features will the bot support?\" type=\"list-multi\" var=\"features\">"
                            "<value>news</value>"
                            "<value>search</value>"
                            "<option label=\"Contests\"><value>contests</value></option>"
                            "<option label=\"News\"><value>news</value></option>"
                            "<option label=\"Polls\"><value>polls</value></option>"
                            "<option label=\"Reminders\"><value>reminders</value></option>"
                            "<option label=\"Search\"><value>search</value></option>"
                        "</field>"
                        "<field label=\"Maximum number of subscribers\" type=\"list-single\" var=\"maxsubs\">"
                            "<value>20</value>"
                            "<option label=\"10\"><value>10</value></option>"
                            "<option label=\"20\"><value>20</value></option>"
                            "<option label=\"30\"><value>30</value></option>"
                            "<option label=\"50\"><value>50</value></option>"
                            "<option label=\"100\"><value>100</value></option>"
                            "<option><value>none</value></option>"
                        "</field>"
                        "<field label=\"People to invite\" type=\"jid-multi\" var=\"invitelist\">"
                            "<desc>Tell all your friends about your new bot!</desc>"
                            "<value>foo@bar.com</value>"
                            "<value>baz@fum.org</value>"
                        "</field>"
                    "</x>"), testling.serialize(form));
        }

        void testSerializeFormItems() {
            FormSerializer testling;
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

            field = std::make_shared<FormField>(FormField::JIDSingleType, JID("benvolio@montague.net"));
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

            field = std::make_shared<FormField>(FormField::JIDSingleType, JID("romeo@montague.net"));
            field->setName("jid");
            secondItem.push_back(field);

            field = std::make_shared<FormField>(FormField::ListSingleType, "male");
            field->setName("x-gender");
            secondItem.push_back(field);

            form->addItem(firstItem);
            form->addItem(secondItem);

            CPPUNIT_ASSERT_EQUAL(std::string(
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
                "</x>"), testling.serialize(form));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(FormSerializerTest);
