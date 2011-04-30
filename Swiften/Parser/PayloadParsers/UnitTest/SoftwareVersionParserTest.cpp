/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/SoftwareVersionParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class SoftwareVersionParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SoftwareVersionParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		SoftwareVersionParserTest() {}

		void testParse() {
			PayloadsParserTester parser;
			
			CPPUNIT_ASSERT(parser.parse(
				"<query xmlns=\"jabber:iq:version\">"
					"<name>myclient</name>"
					"<version>1.0</version>"
					"<os>Mac OS X</os>"
				"</query>"));

			SoftwareVersion* payload = dynamic_cast<SoftwareVersion*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(std::string("myclient"), payload->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("1.0"), payload->getVersion());
			CPPUNIT_ASSERT_EQUAL(std::string("Mac OS X"), payload->getOS());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SoftwareVersionParserTest);
