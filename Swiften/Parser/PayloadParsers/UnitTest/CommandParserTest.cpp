/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Elements/Command.h>

using namespace Swift;

class CommandParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(CommandParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_Result);
		CPPUNIT_TEST(testParse_Form);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<command xmlns='http://jabber.org/protocol/commands' node='list' action='prev' sessionid='myid'/>"
				));

			Command::ref payload = parser.getPayload<Command>();
			CPPUNIT_ASSERT_EQUAL(Command::Prev, payload->getAction());
			CPPUNIT_ASSERT_EQUAL(std::string("list"), payload->getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("myid"), payload->getSessionID());
		}

		void testParse_Result() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<command xmlns='http://jabber.org/protocol/commands' node='config' status='completed' sessionid='myid'>"
					"<note type='warn'>Service 'httpd' has been configured.</note>"
					"<note type='error'>I lied.</note>"
					"<actions execute='next'>"
						"<prev/>"
						"<next/>"
					"</actions>"
				"</command>"
				));

			Command::ref payload = parser.getPayload<Command>();
			CPPUNIT_ASSERT_EQUAL(Command::Completed, payload->getStatus());
			std::vector<Command::Note> notes = payload->getNotes();
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(notes.size()));
			CPPUNIT_ASSERT_EQUAL(Command::Note::Warn, notes[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("Service 'httpd' has been configured."), notes[0].note);
			CPPUNIT_ASSERT_EQUAL(Command::Note::Error, notes[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("I lied."), notes[1].note);
			std::vector<Command::Action> actions = payload->getAvailableActions();
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(actions.size()));
			CPPUNIT_ASSERT_EQUAL(Command::Prev, actions[0]);
			CPPUNIT_ASSERT_EQUAL(Command::Next, actions[1]);
			CPPUNIT_ASSERT_EQUAL(Command::Next, payload->getExecuteAction());
		}

		void testParse_Form() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<command xmlns='http://jabber.org/protocol/commands' node='config' status='completed'>"
					"<x type=\"result\" xmlns=\"jabber:x:data\">"
						"<title>Bot Configuration</title>"
						"<instructions>Hello!</instructions>"
						"<instructions>Fill out this form to configure your new bot!</instructions>"
					"</x>"
				"</command>"
				));

			Command::ref payload = parser.getPayload<Command>();
			Form::ref form = payload->getForm();
			CPPUNIT_ASSERT_EQUAL(std::string("Bot Configuration"), form->getTitle());
			CPPUNIT_ASSERT_EQUAL(std::string("Hello!\nFill out this form to configure your new bot!"), form->getInstructions());
			CPPUNIT_ASSERT_EQUAL(Form::ResultType, form->getType());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(CommandParserTest);
