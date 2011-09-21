/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/MUCAdminPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class MUCAdminPayloadParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(MUCAdminPayloadParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		MUCAdminPayloadParserTest() {}

		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<query xmlns=\"http://jabber.org/protocol/muc#admin\"><item affiliation=\"owner\" role=\"visitor\"><actor jid=\"kev@tester.lit\"/><reason>malice</reason></item></query>"));

			MUCAdminPayload::ref payload = boost::dynamic_pointer_cast<MUCAdminPayload>(parser.getPayload());
			MUCItem item = payload->getItems()[0];
			CPPUNIT_ASSERT_EQUAL(MUCOccupant::Owner, item.affiliation.get());
			CPPUNIT_ASSERT_EQUAL(MUCOccupant::Visitor, item.role.get());
			CPPUNIT_ASSERT_EQUAL(JID("kev@tester.lit"), item.actor.get());
			CPPUNIT_ASSERT_EQUAL(std::string("malice"), item.reason.get());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCAdminPayloadParserTest);



