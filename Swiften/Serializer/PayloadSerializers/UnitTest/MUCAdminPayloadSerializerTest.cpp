/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializers/MUCAdminPayloadSerializer.h>

using namespace Swift;

class MUCAdminPayloadSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(MUCAdminPayloadSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		MUCAdminPayloadSerializerTest() {}

		void testSerialize() {
			MUCAdminPayloadSerializer testling;
			boost::shared_ptr<MUCAdminPayload> admin = boost::make_shared<MUCAdminPayload>();
			MUCItem item;
			item.affiliation = MUCOccupant::Owner;
			item.role = MUCOccupant::Visitor;
			item.reason = "malice";
			item.actor = JID("kev@tester.lit");
			admin->addItem(item);

			CPPUNIT_ASSERT_EQUAL(std::string("<query xmlns=\"http://jabber.org/protocol/muc#admin\"><item affiliation=\"owner\" role=\"visitor\"><actor jid=\"kev@tester.lit\"/><reason>malice</reason></item></query>"), testling.serialize(admin));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCAdminPayloadSerializerTest);
