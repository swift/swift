/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/RosterItemExchangeParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class RosterItemExchangeParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterItemExchangeParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse(
				"<x xmlns=\"http://jabber.org/protocol/rosterx\">"
					"<item action=\"add\" jid=\"foo@bar.com\" name=\"Foo @ Bar\">"
						"<group>Group 1</group>"
						"<group>Group 2</group>"
					"</item>"
					"<item action=\"modify\" jid=\"baz@blo.com\" name=\"Baz\"/>"
				"</x>"));

			RosterItemExchangePayload* payload = dynamic_cast<RosterItemExchangePayload*>(parser.getPayload().get());
			const RosterItemExchangePayload::RosterItemExchangePayloadItems& items = payload->getItems();

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), items.size());

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), items[0].getJID());
			CPPUNIT_ASSERT_EQUAL(std::string("Foo @ Bar"), items[0].getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemExchangePayload::Item::Add, items[0].getAction());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), items[0].getGroups().size());
			CPPUNIT_ASSERT_EQUAL(std::string("Group 1"), items[0].getGroups()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("Group 2"), items[0].getGroups()[1]);

			CPPUNIT_ASSERT_EQUAL(JID("baz@blo.com"), items[1].getJID());
			CPPUNIT_ASSERT_EQUAL(std::string("Baz"), items[1].getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemExchangePayload::Item::Modify, items[1].getAction());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), items[1].getGroups().size());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterItemExchangeParserTest);
