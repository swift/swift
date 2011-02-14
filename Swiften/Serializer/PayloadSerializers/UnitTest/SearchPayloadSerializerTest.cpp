/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/PayloadSerializers/SearchPayloadSerializer.h"

using namespace Swift;

class SearchPayloadSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SearchPayloadSerializerTest);
		CPPUNIT_TEST(testSerialize_Request);
		CPPUNIT_TEST(testSerialize_Items);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize_Request() {
			SearchPayloadSerializer testling;

			SearchPayload::ref payload(new SearchPayload());
			payload->setFirst("Juliet");
			payload->setLast("Capulet");

			CPPUNIT_ASSERT_EQUAL(std::string(
					"<query xmlns=\"jabber:iq:search\">"
						"<first>Juliet</first>"
						"<last>Capulet</last>"
					"</query>"
				), testling.serialize(payload));
		}

		void testSerialize_Items() {
			SearchPayloadSerializer testling;

			SearchPayload::ref payload(new SearchPayload());
			SearchPayload::Item item1;
			item1.jid = JID("juliet@capulet.com");
			item1.first = "Juliet";
			item1.last = "Capulet";
			item1.nick = "JuliC";
			item1.email = "juliet@shakespeare.lit";
			payload->addItem(item1);

			SearchPayload::Item item2;
			item2.jid = JID("tybalt@shakespeare.lit");
			item2.first = "Tybalt";
			item2.last = "Capulet";
			item2.nick = "ty";
			item2.email = "tybalt@shakespeare.lit";
			payload->addItem(item2);

			CPPUNIT_ASSERT_EQUAL(std::string(
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
				), testling.serialize(payload));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SearchPayloadSerializerTest);
