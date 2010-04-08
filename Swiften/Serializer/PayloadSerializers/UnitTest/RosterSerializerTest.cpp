/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/PayloadSerializers/RosterSerializer.h"

using namespace Swift;

class RosterSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		RosterSerializerTest() {}

		void testSerialize() {
			RosterSerializer testling;
			boost::shared_ptr<RosterPayload> roster(new RosterPayload());

			RosterItemPayload item1;
			item1.setJID(JID("foo@bar.com"));
			item1.setName("Foo @ Bar");
			item1.setSubscription(RosterItemPayload::From);
			item1.addGroup("Group 1");
			item1.addGroup("Group 2");
			item1.setSubscriptionRequested();
			roster->addItem(item1);

			RosterItemPayload item2;
			item2.setJID(JID("baz@blo.com"));
			item2.setName("Baz");
			roster->addItem(item2);

			String expectedResult = 
				"<query xmlns=\"jabber:iq:roster\">"
					"<item ask=\"subscribe\" jid=\"foo@bar.com\" name=\"Foo @ Bar\" subscription=\"from\">"
						"<group>Group 1</group>"
						"<group>Group 2</group>"
					"</item>"
					"<item jid=\"baz@blo.com\" name=\"Baz\" subscription=\"none\"/>"
				"</query>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(roster));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterSerializerTest);
