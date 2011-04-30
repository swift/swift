/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/MessageParser.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/UnitTest/StanzaParserTester.h>

using namespace Swift;

class MessageParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(MessageParserTest);
		CPPUNIT_TEST(testParse_Normal);
		CPPUNIT_TEST(testParse_Chat);
		CPPUNIT_TEST(testParse_Error);
		CPPUNIT_TEST(testParse_Groupchat);
		CPPUNIT_TEST(testParse_Headline);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			factoryCollection_ = new PayloadParserFactoryCollection();
		}

		void tearDown() {
			delete factoryCollection_;
		}

		void testParse_Chat() {
			MessageParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<message type=\"chat\"/>"));

			CPPUNIT_ASSERT_EQUAL(Message::Chat, testling.getStanzaGeneric()->getType());
		}

		void testParse_Groupchat() {
			MessageParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<message type=\"groupchat\"/>"));

			CPPUNIT_ASSERT_EQUAL(Message::Groupchat, testling.getStanzaGeneric()->getType());
		}

		void testParse_Error() {
			MessageParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<message type=\"error\"/>"));

			CPPUNIT_ASSERT_EQUAL(Message::Error, testling.getStanzaGeneric()->getType());
		}

		void testParse_Headline() {
			MessageParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<message type=\"headline\"/>"));

			CPPUNIT_ASSERT_EQUAL(Message::Headline, testling.getStanzaGeneric()->getType());
		}

		void testParse_Normal() {
			MessageParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<message/>"));

			CPPUNIT_ASSERT_EQUAL(Message::Normal, testling.getStanzaGeneric()->getType());
		}

		private:
			PayloadParserFactoryCollection* factoryCollection_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MessageParserTest);
