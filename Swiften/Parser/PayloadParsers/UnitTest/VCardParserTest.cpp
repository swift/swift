/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/VCardParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h"

using namespace Swift;

class VCardParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(VCardParserTest);
		CPPUNIT_TEST(testParse_Photo);
		CPPUNIT_TEST(testParse_Nickname);
		CPPUNIT_TEST_SUITE_END();

	public:
		VCardParserTest() {}

		void testParse_Photo() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<vCard xmlns='vcard-temp'>"
					"<PHOTO>"
						"<TYPE>image/jpeg</TYPE>"
						"<BINVAL>"
							"QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ej"
							"EyMzQ1Njc4OTA="
						"</BINVAL>"
					"</PHOTO>"
				"</vCard>"));

			VCard* payload = dynamic_cast<VCard*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(String("image/jpeg"), payload->getPhotoType());
			CPPUNIT_ASSERT_EQUAL(ByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890"), payload->getPhoto());
		}

		void testParse_Nickname() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<vCard xmlns='vcard-temp'>"
					"<NICKNAME>mynick</NICKNAME>"
				"</vCard>"));

			VCard* payload = dynamic_cast<VCard*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(String("mynick"), payload->getNickname());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardParserTest);
