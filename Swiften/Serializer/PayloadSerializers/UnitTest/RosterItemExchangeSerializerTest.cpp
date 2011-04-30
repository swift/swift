/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/RosterItemExchangeSerializer.h>

using namespace Swift;

class RosterItemExchangeSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterItemExchangeSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		RosterItemExchangeSerializerTest() {}

		void testSerialize() {
			RosterItemExchangeSerializer testling;
			boost::shared_ptr<RosterItemExchangePayload> roster(new RosterItemExchangePayload());

			RosterItemExchangePayload::Item item1;
			item1.setJID("foo@bar.com");
			item1.setName("Foo @ Bar");
			item1.setAction(RosterItemExchangePayload::Item::Add);
			item1.addGroup("Group 1");
			item1.addGroup("Group 2");
			roster->addItem(item1);

			RosterItemExchangePayload::Item item2;
			item2.setJID("baz@blo.com");
			item2.setName("Baz");
			item2.setAction(RosterItemExchangePayload::Item::Modify);
			roster->addItem(item2);

			std::string expectedResult = 
				"<x xmlns=\"http://jabber.org/protocol/rosterx\">"
					"<item action=\"add\" jid=\"foo@bar.com\" name=\"Foo @ Bar\">"
						"<group>Group 1</group>"
						"<group>Group 2</group>"
					"</item>"
					"<item action=\"modify\" jid=\"baz@blo.com\" name=\"Baz\"/>"
				"</x>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(roster));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterItemExchangeSerializerTest);
