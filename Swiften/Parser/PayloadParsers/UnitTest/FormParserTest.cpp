/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Elements/Form.h>

using namespace Swift;

class FormParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(FormParserTest);
		CPPUNIT_TEST(testParse_FormInformation);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_FormItems);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse_FormInformation() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<x type=\"submit\" xmlns=\"jabber:x:data\">"
						"<title>Bot Configuration</title>"
						"<instructions>Hello!</instructions>"
						"<instructions>Fill out this form to configure your new bot!</instructions>"
					"</x>"
				));

			Form* payload = dynamic_cast<Form*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(std::string("Bot Configuration"), payload->getTitle());
			CPPUNIT_ASSERT_EQUAL(std::string("Hello!\nFill out this form to configure your new bot!"), payload->getInstructions());
			CPPUNIT_ASSERT_EQUAL(Form::SubmitType, payload->getType());
		}

		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
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
							"<option label=\"Contests\"><value>contests</value></option>"
							"<option label=\"News\"><value>news</value></option>"
							"<option label=\"Polls\"><value>polls</value></option>"
							"<option label=\"Reminders\"><value>reminders</value></option>"
							"<option label=\"Search\"><value>search</value></option>"
							"<value>news</value>"
							"<value>search</value>"
						"</field>"
						"<field label=\"Maximum number of subscribers\" type=\"list-single\" var=\"maxsubs\">"
							"<value>20</value>"
							"<option label=\"10\"><value>10</value></option>"
							"<option label=\"20\"><value>20</value></option>"
							"<option label=\"30\"><value>30</value></option>"
							"<option label=\"50\"><value>50</value></option>"
							"<option label=\"100\"><value>100</value></option>"
							"<option label=\"None\"><value>none</value></option>"
						"</field>"
						"<field label=\"People to invite\" type=\"jid-multi\" var=\"invitelist\">"
							"<desc>Tell all your friends about your new bot!</desc>"
							"<value>foo@bar.com</value>"
							"<value>baz@fum.org</value>"
						"</field>"
						"<field var=\"untyped\">"
							"<value>foo</value>"
						"</field>"
					"</x>"));

			Form* payload = dynamic_cast<Form*>(parser.getPayload().get());

			CPPUNIT_ASSERT_EQUAL(10, static_cast<int>(payload->getFields().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("jabber:bot"), payload->getFields()[0]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("FORM_TYPE"), payload->getFields()[0]->getName());
			CPPUNIT_ASSERT(!payload->getFields()[0]->getRequired());

			CPPUNIT_ASSERT_EQUAL(std::string("Section 1: Bot Info"), payload->getFields()[1]->getValues()[0]);

			CPPUNIT_ASSERT_EQUAL(std::string("The name of your bot"), payload->getFields()[2]->getLabel());

			CPPUNIT_ASSERT_EQUAL(std::string("This is a bot.\nA quite good one actually"), payload->getFields()[3]->getTextMultiValue());

			CPPUNIT_ASSERT_EQUAL(true, payload->getFields()[4]->getBoolValue());
			CPPUNIT_ASSERT(payload->getFields()[4]->getRequired());
			CPPUNIT_ASSERT_EQUAL(std::string("1"),  payload->getFields()[4]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getFields()[6]->getValues().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("news"), payload->getFields()[6]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("search"), payload->getFields()[6]->getValues()[1]);
			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(payload->getFields()[6]->getOptions().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Contests"), payload->getFields()[6]->getOptions()[0].label);
			CPPUNIT_ASSERT_EQUAL(std::string("contests"), payload->getFields()[6]->getOptions()[0].value);
			CPPUNIT_ASSERT_EQUAL(std::string("News"), payload->getFields()[6]->getOptions()[1].label);
			CPPUNIT_ASSERT_EQUAL(std::string("news"), payload->getFields()[6]->getOptions()[1].value);

			CPPUNIT_ASSERT_EQUAL(std::string("20"), payload->getFields()[7]->getValues()[0]);

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), payload->getFields()[8]->getJIDMultiValue(0));
			CPPUNIT_ASSERT_EQUAL(JID("baz@fum.org"), payload->getFields()[8]->getJIDMultiValue(1));
			CPPUNIT_ASSERT_EQUAL(std::string("Tell all your friends about your new bot!"), payload->getFields()[8]->getDescription());

			CPPUNIT_ASSERT_EQUAL(std::string("foo"), payload->getFields()[9]->getValues()[0]);
		}

		void testParse_FormItems() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<x xmlns='jabber:x:data' type='result'>"
					"<field type='hidden' var='FORM_TYPE'>"
						"<value>jabber:iq:search</value>"
					"</field>"
					"<reported>"
						"<field var='first' label='Given Name' type='text-single'/>"
						"<field var='last' label='Family Name' type='text-single'/>"
						"<field var='jid' label='Jabber ID' type='jid-single'/>"
						"<field var='x-gender' label='Gender' type='list-single'/>"
					"</reported>"
					"<item>"
						"<field var='first'><value>Benvolio</value></field>"
						"<field var='last'><value>Montague</value></field>"
						"<field var='jid'><value>benvolio@montague.net</value></field>"
						"<field var='x-gender'><value>male</value></field>"
					"</item>"
					"<item>"
						"<field var='first'><value>Romeo</value></field>"
						"<field var='last'><value>Montague</value></field>"
						"<field var='jid'><value>romeo@montague.net</value></field>"
						"<field var='x-gender'><value>male</value></field>"
					"</item>"
				"</x>"));

			Form* dataForm = dynamic_cast<Form*>(parser.getPayload().get());
			CPPUNIT_ASSERT(dataForm);

			Form::FormItem reported = dataForm->getReportedFields();
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), reported.size());

			std::vector<Form::FormItem> items = dataForm->getItems();
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), items.size());

			Form::FormItem item = items[0];
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), item.size());

			CPPUNIT_ASSERT_EQUAL(std::string("Benvolio"), item[0]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("first"), item[0]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("Montague"), item[1]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("last"), item[1]->getName());
			boost::shared_ptr<FormField> jidField = item[2];
			CPPUNIT_ASSERT_EQUAL(JID("benvolio@montague.net"), jidField->getJIDSingleValue());
			CPPUNIT_ASSERT_EQUAL(std::string("jid"), item[2]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("male"), item[3]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("x-gender"), item[3]->getName());

			item = items[1];
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), item.size());

			CPPUNIT_ASSERT_EQUAL(std::string("Romeo"), item[0]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("first"), item[0]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("Montague"), item[1]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("last"), item[1]->getName());
			jidField = item[2];
			CPPUNIT_ASSERT(jidField);
			CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.net"), jidField->getJIDSingleValue());
			CPPUNIT_ASSERT_EQUAL(std::string("jid"), item[2]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("male"), item[3]->getValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("x-gender"), item[3]->getName());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(FormParserTest);
