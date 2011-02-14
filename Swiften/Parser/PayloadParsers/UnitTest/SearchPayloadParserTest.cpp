/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h"
#include "Swiften/Elements/SearchPayload.h"

using namespace Swift;

class SearchPayloadParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SearchPayloadParserTest);
		CPPUNIT_TEST(testParse_FormRequestResponse);
		CPPUNIT_TEST(testParse_Results);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse_FormRequestResponse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
					"<query xmlns=\"jabber:iq:search\">"
						"<instructions>Foo</instructions>"
						"<first/>"
						"<last/>"
					"</query>"
				));

			SearchPayload::ref payload = parser.getPayload<SearchPayload>();
			CPPUNIT_ASSERT_EQUAL(std::string("Foo"), *payload->getInstructions());
			CPPUNIT_ASSERT(payload->getFirst());
			CPPUNIT_ASSERT(payload->getLast());
			CPPUNIT_ASSERT(!payload->getNick());
		}

		void testParse_Results() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
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
				));

			SearchPayload::ref payload = parser.getPayload<SearchPayload>();
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getItems().size()));
			CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.com"), payload->getItems()[0].jid);
			CPPUNIT_ASSERT_EQUAL(std::string("Juliet"), payload->getItems()[0].first);
			CPPUNIT_ASSERT_EQUAL(std::string("Capulet"), payload->getItems()[0].last);
			CPPUNIT_ASSERT_EQUAL(std::string("JuliC"), payload->getItems()[0].nick);
			CPPUNIT_ASSERT_EQUAL(std::string("juliet@shakespeare.lit"), payload->getItems()[0].email);
			CPPUNIT_ASSERT_EQUAL(JID("tybalt@shakespeare.lit"), payload->getItems()[1].jid);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SearchPayloadParserTest);
