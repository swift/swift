/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/ErrorParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h"

using namespace Swift;

class ErrorParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ErrorParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<error type=\"modify\">"
					"<bad-request xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/>"
					"<text xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\">boo</text>"
				"</error>"));

			ErrorPayload::ref payload = boost::dynamic_pointer_cast<ErrorPayload>(parser.getPayload());
			CPPUNIT_ASSERT_EQUAL(ErrorPayload::BadRequest, payload->getCondition());
			CPPUNIT_ASSERT_EQUAL(ErrorPayload::Modify, payload->getType());
			CPPUNIT_ASSERT_EQUAL(std::string("boo"), payload->getText());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ErrorParserTest);
