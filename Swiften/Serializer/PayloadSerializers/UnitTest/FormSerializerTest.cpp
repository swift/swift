/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
using namespace Swift;

class FormSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(FormSerializerTest);
		CPPUNIT_TEST(testSerializeFormInformation);
		CPPUNIT_TEST(testSerializeFields);
		CPPUNIT_TEST(testSerializeFormItems);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerializeFormInformation() {
			FormSerializer testling;
			boost::shared_ptr<Form> form(new Form(Form::FormType));
			form->setTitle("Bot Configuration");
			form->setInstructions("Hello!\nFill out this form to configure your new bot!");

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<x type=\"form\" xmlns=\"jabber:x:data\">"
						"<title>Bot Configuration</title>"
						"<instructions>Hello!</instructions>"
						"<instructions>Fill out this form to configure your new bot!</instructions>"
					"</x>"), testling.serialize(form));
		}

		void testSerializeFields() {
			FormSerializer testling;
			boost::shared_ptr<Form> form(new Form(Form::FormType));

			FormField::ref field = HiddenFormField::create("jabber:bot");
			field->setName("FORM_TYPE");
			form->addField(field);

			form->addField(FixedFormField::create("Section 1: Bot Info"));

			field = TextSingleFormField::create();
			field->setName("botname");
			field->setLabel("The name of your bot");
			form->addField(field);

			field = TextMultiFormField::create("This is a bot.\nA quite good one actually");
			field->setName("description");
			field->setLabel("Helpful description of your bot");
			form->addField(field);

			field = BooleanFormField::create(true);
			field->setName("public");
			field->setLabel("Public bot?");
			field->setRequired(true);
			form->addField(field);

			field = TextPrivateFormField::create();
			field->setName("password");
			field->setLabel("Password for special access");
			form->addField(field);

			std::vector<std::string> values;
			values.push_back("news");
			values.push_back("search");
			field = ListMultiFormField::create(values);
			field->setName("features");
			field->setLabel("What features will the bot support?");
			field->addOption(FormField::Option("Contests", "contests"));
			field->addOption(FormField::Option("News", "news"));
			field->addOption(FormField::Option("Polls", "polls"));
			field->addOption(FormField::Option("Reminders", "reminders"));
			field->addOption(FormField::Option("Search", "search"));
			form->addField(field);

			field = ListSingleFormField::create("20");
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
			jids.push_back(JID("foo@bar.com"));
			jids.push_back(JID("baz@fum.org"));
			field = JIDMultiFormField::create(jids);
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
			boost::shared_ptr<Form> form(new Form(Form::ResultType));


			FormField::ref field = HiddenFormField::create("jabber:iq:search");
			field->setName("FORM_TYPE");
			form->addField(field);

			// reported fields
			field = TextSingleFormField::create();
			field->setName("first");
			field->setLabel("Given Name");
			form->addReportedField(field);

			field = TextSingleFormField::create();
			field->setName("last");
			field->setLabel("Family Name");
			form->addReportedField(field);

			field = JIDSingleFormField::create();
			field->setName("jid");
			field->setLabel("Jabber ID");
			form->addReportedField(field);

			field = ListSingleFormField::create();
			field->setName("x-gender");
			field->setLabel("Gender");
			form->addReportedField(field);

			Form::FormItem firstItem;
			field = TextSingleFormField::create("Benvolio");
			field->setName("first");
			firstItem.push_back(field);

			field = TextSingleFormField::create("Montague");
			field->setName("last");
			firstItem.push_back(field);

			field = JIDSingleFormField::create(JID("benvolio@montague.net"));
			field->setName("jid");
			firstItem.push_back(field);

			field = ListSingleFormField::create("male");
			field->setName("x-gender");
			firstItem.push_back(field);

			Form::FormItem secondItem;
			field = TextSingleFormField::create("Romeo");
			field->setName("first");
			secondItem.push_back(field);

			field = TextSingleFormField::create("Montague");
			field->setName("last");
			secondItem.push_back(field);

			field = JIDSingleFormField::create(JID("romeo@montague.net"));
			field->setName("jid");
			secondItem.push_back(field);

			field = ListSingleFormField::create("male");
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
