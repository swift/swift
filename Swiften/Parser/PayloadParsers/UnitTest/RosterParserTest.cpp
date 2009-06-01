#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/RosterParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/XMLPayloadParser.h"

using namespace Swift;

class RosterParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		RosterParserTest() {}

		void testParse() {
			RosterParser testling;
			XMLPayloadParser parser(&testling);
			parser.parse(
				"<query xmlns='jabber:iq:roster'>"
				"	<item jid='foo@bar.com' name='Foo @ Bar' subscription='from' ask='subscribe'>"
				"		<group>Group 1</group>"
				"		<group>Group 2</group>"
				"	</item>"
				" <item jid='baz@blo.com' name='Baz'/>"
				"</query>");

			RosterPayload* payload = dynamic_cast<RosterPayload*>(testling.getPayload().get());
			const RosterPayload::RosterItemPayloads& items = payload->getItems();

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), items.size());

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), items[0].getJID());
			CPPUNIT_ASSERT_EQUAL(String("Foo @ Bar"), items[0].getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemPayload::From, items[0].getSubscription());
			CPPUNIT_ASSERT(items[0].getSubscriptionRequested());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), items[0].getGroups().size());
			CPPUNIT_ASSERT_EQUAL(String("Group 1"), items[0].getGroups()[0]);
			CPPUNIT_ASSERT_EQUAL(String("Group 2"), items[0].getGroups()[1]);

			CPPUNIT_ASSERT_EQUAL(JID("baz@blo.com"), items[1].getJID());
			CPPUNIT_ASSERT_EQUAL(String("Baz"), items[1].getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemPayload::None, items[1].getSubscription());
			CPPUNIT_ASSERT(!items[1].getSubscriptionRequested());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), items[1].getGroups().size());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterParserTest);
