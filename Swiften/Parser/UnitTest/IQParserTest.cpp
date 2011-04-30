/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/IQParser.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/UnitTest/StanzaParserTester.h>

using namespace Swift;

class IQParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(IQParserTest);
		CPPUNIT_TEST(testParse_Set);
		CPPUNIT_TEST(testParse_Get);
		CPPUNIT_TEST(testParse_Result);
		CPPUNIT_TEST(testParse_Error);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			factoryCollection_ = new PayloadParserFactoryCollection();
		}

		void tearDown() {
			delete factoryCollection_;
		}

		void testParse_Set() {
			IQParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<iq type=\"set\"/>"));

			CPPUNIT_ASSERT_EQUAL(IQ::Set, testling.getStanzaGeneric()->getType());
		}

		void testParse_Get() {
			IQParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<iq type=\"get\"/>"));

			CPPUNIT_ASSERT_EQUAL(IQ::Get, testling.getStanzaGeneric()->getType());
		}

		void testParse_Result() {
			IQParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<iq type=\"result\"/>"));

			CPPUNIT_ASSERT_EQUAL(IQ::Result, testling.getStanzaGeneric()->getType());
		}

		void testParse_Error() {
			IQParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<iq type=\"error\"/>"));

			CPPUNIT_ASSERT_EQUAL(IQ::Error, testling.getStanzaGeneric()->getType());
		}

		private:
			PayloadParserFactoryCollection* factoryCollection_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IQParserTest);
