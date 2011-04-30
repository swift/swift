/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/ReplaceParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class ReplaceParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ReplaceParserTest);
		CPPUNIT_TEST(testParseTrivial);
		CPPUNIT_TEST(testParseChild);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParseTrivial() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse("<replace id='bad1' xmlns='http://swift.im/protocol/replace'/>"));
			Replace::ref payload = boost::dynamic_pointer_cast <Replace>(parser.getPayload());
			CPPUNIT_ASSERT_EQUAL(std::string("bad1"), payload->getID());
		}
		void testParseChild() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse("<replace id='bad1' xmlns='http://swift.im/protocol/replace' ><child xmlns='blah' id=\"hi\"/></replace>"));
			Replace::ref payload = boost::dynamic_pointer_cast <Replace>(parser.getPayload());
			CPPUNIT_ASSERT_EQUAL(std::string("bad1"), payload->getID());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ReplaceParserTest);
