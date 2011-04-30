/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/StatusShowParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class StatusShowParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StatusShowParserTest);
		CPPUNIT_TEST(testParse_Invalid);
		CPPUNIT_TEST(testParse_Away);
		CPPUNIT_TEST(testParse_FFC);
		CPPUNIT_TEST(testParse_XA);
		CPPUNIT_TEST(testParse_DND);
		CPPUNIT_TEST_SUITE_END();

	public:
		StatusShowParserTest() {}

		void testParse_Invalid() {
			PayloadsParserTester parser;
			
			CPPUNIT_ASSERT(parser.parse("<show>invalid</show>"));

			StatusShow* payload = dynamic_cast<StatusShow*>(parser.getPayload().get());
			CPPUNIT_ASSERT(StatusShow::Online == payload->getType());
		}

		void testParse_Away() {
			PayloadsParserTester parser;
			
			CPPUNIT_ASSERT(parser.parse("<show>away</show>"));

			StatusShow* payload = dynamic_cast<StatusShow*>(parser.getPayload().get());
			CPPUNIT_ASSERT(StatusShow::Away == payload->getType());
		}

		void testParse_FFC() {
			PayloadsParserTester parser;
			
			CPPUNIT_ASSERT(parser.parse("<show>chat</show>"));

			StatusShow* payload = dynamic_cast<StatusShow*>(parser.getPayload().get());
			CPPUNIT_ASSERT(StatusShow::FFC == payload->getType());
		}

		void testParse_XA() {
			PayloadsParserTester parser;
			
			CPPUNIT_ASSERT(parser.parse("<show>xa</show>"));

			StatusShow* payload = dynamic_cast<StatusShow*>(parser.getPayload().get());
			CPPUNIT_ASSERT(StatusShow::XA == payload->getType());
		}

		void testParse_DND() {
			PayloadsParserTester parser;
			
			CPPUNIT_ASSERT(parser.parse("<show>dnd</show>"));

			StatusShow* payload = dynamic_cast<StatusShow*>(parser.getPayload().get());
			CPPUNIT_ASSERT(StatusShow::DND == payload->getType());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StatusShowParserTest);
