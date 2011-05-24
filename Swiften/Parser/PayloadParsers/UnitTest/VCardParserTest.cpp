/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/VCardParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class VCardParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(VCardParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_Photo);
		CPPUNIT_TEST(testParse_Nickname);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<vCard xmlns=\"vcard-temp\">"
					"<VERSION>2.0</VERSION>"
					"<FN>Alice In Wonderland</FN>"
					"<N>"
						"<FAMILY>Wonderland</FAMILY>"
						"<GIVEN>Alice</GIVEN>"
						"<MIDDLE>In</MIDDLE>"
						"<PREFIX>Mrs</PREFIX>"
						"<SUFFIX>PhD</SUFFIX>"
					"</N>"
					"<EMAIL>"
						"<USERID>alice@wonderland.lit</USERID>"
						"<HOME/>"
						"<INTERNET/>"
						"<PREF/>"
					"</EMAIL>"
					"<EMAIL>"
						"<USERID>alice@teaparty.lit</USERID>"
						"<WORK/>"
						"<X400/>"
					"</EMAIL>"
					"<NICKNAME>DreamGirl</NICKNAME>"
					"<BDAY>1234</BDAY>"
					"<MAILER>mutt</MAILER>"
				"</vCard>"));

			boost::shared_ptr<VCard> payload = boost::dynamic_pointer_cast<VCard>(parser.getPayload());
			CPPUNIT_ASSERT_EQUAL(std::string("2.0"), payload->getVersion());
			CPPUNIT_ASSERT_EQUAL(std::string("Alice In Wonderland"), payload->getFullName());
			CPPUNIT_ASSERT_EQUAL(std::string("Alice"), payload->getGivenName());
			CPPUNIT_ASSERT_EQUAL(std::string("In"), payload->getMiddleName());
			CPPUNIT_ASSERT_EQUAL(std::string("Wonderland"), payload->getFamilyName());
			CPPUNIT_ASSERT_EQUAL(std::string("Mrs"), payload->getPrefix());
			CPPUNIT_ASSERT_EQUAL(std::string("PhD"), payload->getSuffix());
			CPPUNIT_ASSERT_EQUAL(std::string("DreamGirl"), payload->getNickname());
			CPPUNIT_ASSERT_EQUAL(std::string("<BDAY xmlns=\"vcard-temp\">1234</BDAY><MAILER xmlns=\"vcard-temp\">mutt</MAILER>"), payload->getUnknownContent());
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getEMailAddresses().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("alice@wonderland.lit"), payload->getEMailAddresses()[0].address);
			CPPUNIT_ASSERT(payload->getEMailAddresses()[0].isHome);
			CPPUNIT_ASSERT(payload->getEMailAddresses()[0].isInternet);
			CPPUNIT_ASSERT(payload->getEMailAddresses()[0].isPreferred);
			CPPUNIT_ASSERT(!payload->getEMailAddresses()[0].isWork);
			CPPUNIT_ASSERT(!payload->getEMailAddresses()[0].isX400);
			CPPUNIT_ASSERT_EQUAL(std::string("alice@teaparty.lit"), payload->getEMailAddresses()[1].address);
			CPPUNIT_ASSERT(!payload->getEMailAddresses()[1].isHome);
			CPPUNIT_ASSERT(!payload->getEMailAddresses()[1].isInternet);
			CPPUNIT_ASSERT(!payload->getEMailAddresses()[1].isPreferred);
			CPPUNIT_ASSERT(payload->getEMailAddresses()[1].isWork);
			CPPUNIT_ASSERT(payload->getEMailAddresses()[1].isX400);
		}

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
			CPPUNIT_ASSERT_EQUAL(std::string("image/jpeg"), payload->getPhotoType());
			CPPUNIT_ASSERT_EQUAL(createByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890"), payload->getPhoto());
		}

		void testParse_Nickname() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<vCard xmlns='vcard-temp'>"
					"<NICKNAME>mynick</NICKNAME>"
				"</vCard>"));

			VCard* payload = dynamic_cast<VCard*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(std::string("mynick"), payload->getNickname());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardParserTest);
