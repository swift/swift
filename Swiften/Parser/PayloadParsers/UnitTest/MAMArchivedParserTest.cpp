/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/PayloadParsers/MAMArchivedParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class MAMArchivedParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(MAMArchivedParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse(
				"<archived by=\"juliet@capulet.lit\" id=\"28482-98726-73623\" xmlns=\"urn:xmpp:mam:0\"/>"));

			boost::shared_ptr<MAMArchived> payload = parser.getPayload<MAMArchived>();
			CPPUNIT_ASSERT(!!payload);
			CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.lit"), payload->getBy());
			CPPUNIT_ASSERT_EQUAL(std::string("28482-98726-73623"), payload->getID());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(MAMArchivedParserTest);
