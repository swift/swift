/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/ErrorParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Elements/Delay.h>

using namespace Swift;

class ErrorParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ErrorParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParseWithPayload);
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
			CPPUNIT_ASSERT(!payload->getPayload());
		}

		void testParseWithPayload() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
					"<error type=\"modify\">"
					"<bad-request xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/>"
					"<delay xmlns='urn:xmpp:delay' from='juliet@capulet.com/balcony' stamp='2002-09-10T23:41:07Z'/>"
					"<text xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\">boo</text>"
					"</error>"));

			ErrorPayload::ref payload = boost::dynamic_pointer_cast<ErrorPayload>(parser.getPayload());
			CPPUNIT_ASSERT_EQUAL(ErrorPayload::BadRequest, payload->getCondition());
			CPPUNIT_ASSERT_EQUAL(ErrorPayload::Modify, payload->getType());
			CPPUNIT_ASSERT_EQUAL(std::string("boo"), payload->getText());
			CPPUNIT_ASSERT(boost::dynamic_pointer_cast<Delay>(payload->getPayload()));
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION(ErrorParserTest);
