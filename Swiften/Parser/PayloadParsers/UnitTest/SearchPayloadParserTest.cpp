/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Elements/SearchPayload.h>

using namespace Swift;

class SearchPayloadParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SearchPayloadParserTest);
		CPPUNIT_TEST(testParse_FormRequestResponse);
		CPPUNIT_TEST(testParse_Results);
		CPPUNIT_TEST(testParse_FormRequestResponse_XDATA);
		CPPUNIT_TEST(testParse_Results_XDATA);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse_FormRequestResponse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
					"<query xmlns=\"jabber:iq:search\">"
						"<instructions>Foo</instructions>"
						"<first/>"
						"<last/>"
					"</query>"
				));

			SearchPayload::ref payload = parser.getPayload<SearchPayload>();
			CPPUNIT_ASSERT_EQUAL(std::string("Foo"), *payload->getInstructions());
			CPPUNIT_ASSERT(payload->getFirst());
			CPPUNIT_ASSERT(payload->getLast());
			CPPUNIT_ASSERT(!payload->getNick());
		}

		void testParse_Results() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
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
				));

			SearchPayload::ref payload = parser.getPayload<SearchPayload>();
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getItems().size()));
			CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.com"), payload->getItems()[0].jid);
			CPPUNIT_ASSERT_EQUAL(std::string("Juliet"), payload->getItems()[0].first);
			CPPUNIT_ASSERT_EQUAL(std::string("Capulet"), payload->getItems()[0].last);
			CPPUNIT_ASSERT_EQUAL(std::string("JuliC"), payload->getItems()[0].nick);
			CPPUNIT_ASSERT_EQUAL(std::string("juliet@shakespeare.lit"), payload->getItems()[0].email);
			CPPUNIT_ASSERT_EQUAL(JID("tybalt@shakespeare.lit"), payload->getItems()[1].jid);
		}

		void testParse_FormRequestResponse_XDATA() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
							"<query xmlns='jabber:iq:search'>"
								"<instructions>"
									"Use the enclosed form to search. If your Jabber client does not"
									" support Data Forms, visit http://shakespeare.lit/"
								"</instructions>"
								"<x xmlns='jabber:x:data' type='form'>"
									"<title>User Directory Search</title>"
									"<instructions>"
										"Please provide the following information"
										" to search for Shakespearean characters."
									"</instructions>"
									"<field type='hidden'"
										" var='FORM_TYPE'>"
										"<value>jabber:iq:search</value>"
									"</field>"
									"<field type='text-single'"
									" label='Given Name'"
									" var='first'/>"
									"<field type='text-single'"
									" label='Family Name'"
									" var='last'/>"
									"<field type='list-single'"
									" label='Gender'"
									" var='x-gender'>"
										"<option label='Male'><value>male</value></option>"
										"<option label='Female'><value>female</value></option>"
									"</field>"
								"</x>"
							"</query>"
				));

			SearchPayload::ref payload = parser.getPayload<SearchPayload>();
			CPPUNIT_ASSERT_EQUAL(std::string("Use the enclosed form to search. If your Jabber client does not"
											 " support Data Forms, visit http://shakespeare.lit/"), *payload->getInstructions());
			CPPUNIT_ASSERT(payload->getForm());
			CPPUNIT_ASSERT_EQUAL(std::string("Please provide the following information"
											 " to search for Shakespearean characters."), payload->getForm()->getInstructions());
		}

		void testParse_Results_XDATA() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<query xmlns='jabber:iq:search'>"
										"	<x xmlns='jabber:x:data' type='result'>"
										"		<field type='hidden' var='FORM_TYPE'>"
										"			<value>jabber:iq:search</value>"
										"		</field>"
										"		<reported>"
										"			<field var='first' label='Given Name' type='text-single'/>"
										"			<field var='last' label='Family Name' type='text-single'/>"
										"			<field var='jid' label='Jabber ID' type='jid-single'/>"
										"			<field var='x-gender' label='Gender' type='list-single'/>"
										"		</reported>"
										"		<item>"
										"			<field var='first'><value>Benvolio</value></field>"
										"			<field var='last'><value>Montague</value></field>"
										"			<field var='jid'><value>benvolio@montague.net</value></field>"
										"			<field var='x-gender'><value>male</value></field>"
										"		</item>"
										"		<item>"
										"			<field var='first'><value>Romeo</value></field>"
										"			<field var='last'><value>Montague</value></field>"
										"			<field var='jid'><value>romeo@montague.net</value></field>"
										"			<field var='x-gender'><value>male</value></field>"
										"		</item>"
										"	</x>"
										"</query>"));
			SearchPayload::ref payload = parser.getPayload<SearchPayload>();
			CPPUNIT_ASSERT(payload);

			Form::ref dataForm = payload->getForm();
			CPPUNIT_ASSERT(dataForm);

			Form::FormItem reported = dataForm->getReportedFields();
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), reported.size());

			std::vector<Form::FormItem> items = dataForm->getItems();
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), items.size());

			Form::FormItem item = items[0];
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), item.size());

			CPPUNIT_ASSERT_EQUAL(std::string("Benvolio"), item[0]->getRawValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("first"), item[0]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("Montague"), item[1]->getRawValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("last"), item[1]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("benvolio@montague.net"), item[2]->getRawValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("jid"), item[2]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("male"), item[3]->getRawValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("x-gender"), item[3]->getName());

			item = items[1];
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), item.size());

			CPPUNIT_ASSERT_EQUAL(std::string("Romeo"), item[0]->getRawValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("first"), item[0]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("Montague"), item[1]->getRawValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("last"), item[1]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("romeo@montague.net"), item[2]->getRawValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("jid"), item[2]->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("male"), item[3]->getRawValues()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("x-gender"), item[3]->getName());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SearchPayloadParserTest);
