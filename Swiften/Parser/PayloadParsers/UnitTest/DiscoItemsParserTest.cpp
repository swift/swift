/*
 * Copyright (c) 2012 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/DiscoItemsParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class DiscoItemsParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DiscoItemsParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<query xmlns='http://jabber.org/protocol/disco#items' node='http://jabber.org/protocol/commands'>"
					"<item jid='responder@domain' node='list' name='List Service Configurations'/>"
					"<item jid='responder@domain' node='config' name='Configure Service'/>"
				"</query>"));

			boost::shared_ptr<DiscoItems> payload = boost::dynamic_pointer_cast<DiscoItems>(parser.getPayload());
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getItems().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("List Service Configurations"), payload->getItems()[0].getName());
			CPPUNIT_ASSERT_EQUAL(std::string("list"), payload->getItems()[0].getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("responder@domain"), payload->getItems()[0].getJID().toString());
			CPPUNIT_ASSERT_EQUAL(std::string("Configure Service"), payload->getItems()[1].getName());
			CPPUNIT_ASSERT_EQUAL(std::string("config"), payload->getItems()[1].getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("responder@domain"), payload->getItems()[1].getJID().toString());
			CPPUNIT_ASSERT_EQUAL(std::string("http://jabber.org/protocol/commands"), payload->getNode());
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION(DiscoItemsParserTest);
