/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/RosterParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class RosterParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_ItemWithUnknownContent);
		CPPUNIT_TEST(testParse_WithVersion);
		CPPUNIT_TEST(testParse_WithEmptyVersion);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse(
				"<query xmlns='jabber:iq:roster'>"
				"	<item jid='foo@bar.com' name='Foo @ Bar' subscription='from' ask='subscribe'>"
				"		<group>Group 1</group>"
				"		<group>Group 2</group>"
				"	</item>"
				" <item jid='baz@blo.com' name='Baz'/>"
				"</query>"));

			RosterPayload* payload = dynamic_cast<RosterPayload*>(parser.getPayload().get());

			CPPUNIT_ASSERT(!payload->getVersion());
			const RosterPayload::RosterItemPayloads& items = payload->getItems();

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), items.size());

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), items[0].getJID());
			CPPUNIT_ASSERT_EQUAL(std::string("Foo @ Bar"), items[0].getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemPayload::From, items[0].getSubscription());
			CPPUNIT_ASSERT(items[0].getSubscriptionRequested());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), items[0].getGroups().size());
			CPPUNIT_ASSERT_EQUAL(std::string("Group 1"), items[0].getGroups()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("Group 2"), items[0].getGroups()[1]);

			CPPUNIT_ASSERT_EQUAL(JID("baz@blo.com"), items[1].getJID());
			CPPUNIT_ASSERT_EQUAL(std::string("Baz"), items[1].getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemPayload::None, items[1].getSubscription());
			CPPUNIT_ASSERT(!items[1].getSubscriptionRequested());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), items[1].getGroups().size());
		}

		void testParse_ItemWithUnknownContent() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse(
				"<query xmlns='jabber:iq:roster'>"
				"	<item jid='foo@bar.com' name='Foo @ Bar' subscription='from' ask='subscribe'>"
				"		<group>Group 1</group>"
				"		<foo xmlns=\"http://example.com\"><bar>Baz</bar></foo>"
				"		<group>Group 2</group>"
				"		<baz><fum>foo</fum></baz>"
				"	</item>"
				"</query>"));

			RosterPayload* payload = dynamic_cast<RosterPayload*>(parser.getPayload().get());
			const RosterPayload::RosterItemPayloads& items = payload->getItems();

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), items.size());
			CPPUNIT_ASSERT_EQUAL(std::string("Group 1"), items[0].getGroups()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("Group 2"), items[0].getGroups()[1]);
			CPPUNIT_ASSERT_EQUAL(std::string(
				"<foo xmlns=\"http://example.com\"><bar xmlns=\"http://example.com\">Baz</bar></foo>"
				"<baz xmlns=\"jabber:iq:roster\"><fum xmlns=\"jabber:iq:roster\">foo</fum></baz>"
				), items[0].getUnknownContent());
		}

		void testParse_WithVersion() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse("<query xmlns='jabber:iq:roster' ver='ver10'/>"));

			RosterPayload* payload = dynamic_cast<RosterPayload*>(parser.getPayload().get());
			CPPUNIT_ASSERT(payload->getVersion());
			CPPUNIT_ASSERT_EQUAL(std::string("ver10"), *payload->getVersion());
		}

		void testParse_WithEmptyVersion() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse("<query xmlns='jabber:iq:roster' ver=''/>"));

			RosterPayload* payload = dynamic_cast<RosterPayload*>(parser.getPayload().get());
			CPPUNIT_ASSERT(payload->getVersion());
			CPPUNIT_ASSERT_EQUAL(std::string(""), *payload->getVersion());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterParserTest);
