/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/Message.h>
#include <Swiften/Parser/PayloadParsers/DeliveryReceiptParser.h>
#include <Swiften/Parser/PayloadParsers/DeliveryReceiptRequestParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class DeliveryReceiptParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DeliveryReceiptParserTest);
		CPPUNIT_TEST(testParseXEP0184Example3);
		CPPUNIT_TEST(testParseXEP0184Example4);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParseXEP0184Example3() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse("<request xmlns='urn:xmpp:receipts'/>"));

			DeliveryReceiptRequest::ref request = boost::dynamic_pointer_cast<DeliveryReceiptRequest>(parser.getPayload());

			CPPUNIT_ASSERT(request);
		}

		void testParseXEP0184Example4() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse("<received xmlns='urn:xmpp:receipts' id='richard2-4.1.247'/>"));

			DeliveryReceipt::ref receipt = boost::dynamic_pointer_cast<DeliveryReceipt>(parser.getPayload());

			CPPUNIT_ASSERT_EQUAL(std::string("richard2-4.1.247"), receipt->getReceivedID());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(DeliveryReceiptParserTest);

