/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/foreach.h>
#include <Swiften/Parser/PayloadParsers/MUCUserPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class MUCUserPayloadParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(MUCUserPayloadParserTest);
		CPPUNIT_TEST(testParseEmpty);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		MUCUserPayloadParserTest() {}

		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<x xmlns=\"http://jabber.org/protocol/muc#user\"><status code='110'/><item affiliation=\"owner\" role=\"visitor\"><actor jid=\"kev@tester.lit\"/><reason>malice</reason></item><status code='210'/></x>"));

			bool found110 = false;
			bool found210 = false;

			MUCUserPayload::ref payload = boost::dynamic_pointer_cast<MUCUserPayload>(parser.getPayload());

			foreach (MUCUserPayload::StatusCode status, payload->getStatusCodes()) {
				if (status.code == 110) found110 = true;
				if (status.code == 210) found210 = true;
			}

			MUCItem item = payload->getItems()[0];
			CPPUNIT_ASSERT_EQUAL(MUCOccupant::Owner, item.affiliation.get());
			CPPUNIT_ASSERT_EQUAL(MUCOccupant::Visitor, item.role.get());
			CPPUNIT_ASSERT_EQUAL(JID("kev@tester.lit"), item.actor.get());
			CPPUNIT_ASSERT_EQUAL(std::string("malice"), item.reason.get());
			CPPUNIT_ASSERT(found110);
			CPPUNIT_ASSERT(found210);
		}

		void testParseEmpty() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse("<x xmlns=\"http://jabber.org/protocol/muc#user\"/>"));

			MUCUserPayload::ref payload = boost::dynamic_pointer_cast<MUCUserPayload>(parser.getPayload());
			CPPUNIT_ASSERT(payload);
			CPPUNIT_ASSERT(payload->getItems().empty());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCUserPayloadParserTest);
