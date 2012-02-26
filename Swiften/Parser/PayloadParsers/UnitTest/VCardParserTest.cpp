/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <boost/date_time/posix_time/posix_time.hpp>

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
					"<TEL>"
						"<NUMBER>555-6273</NUMBER>"
						"<HOME/>"
						"<VOICE/>"
					"</TEL>"
					"<ADR>"
						"<LOCALITY>Any Town</LOCALITY>"
						"<STREET>Fake Street 123</STREET>"
						"<PCODE>12345</PCODE>"
						"<CTRY>USA</CTRY>"
						"<HOME/>"
					"</ADR>"
					"<LABEL>"
						"<LINE>Fake Street 123</LINE>"
						"<LINE>12345 Any Town</LINE>"
						"<LINE>USA</LINE>"
						"<HOME/>"
					"</LABEL>"
					"<NICKNAME>DreamGirl</NICKNAME>"
					"<BDAY>1865-05-04</BDAY>"
					"<JID>alice@teaparty.lit</JID>"
					"<JID>alice@wonderland.lit</JID>"
					"<DESC>I once fell down a rabbit hole.</DESC>"
					"<ORG>"
						"<ORGNAME>Alice In Wonderland Inc.</ORGNAME>"
					"</ORG>"
					"<TITLE>Some Title</TITLE>"
					"<ROLE>Main Character</ROLE>"
					"<URL>http://wonderland.lit/~alice</URL>"
					"<URL>http://teaparty.lit/~alice2</URL>"
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
			CPPUNIT_ASSERT_EQUAL(boost::posix_time::ptime(boost::gregorian::date(1865, 5, 4)), payload->getBirthday());
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

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(payload->getTelephones().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("555-6273"), payload->getTelephones()[0].number);
			CPPUNIT_ASSERT(payload->getTelephones()[0].isHome);
			CPPUNIT_ASSERT(payload->getTelephones()[0].isVoice);
			CPPUNIT_ASSERT(!payload->getTelephones()[0].isPreferred);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(payload->getAddresses().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Any Town"), payload->getAddresses()[0].locality);
			CPPUNIT_ASSERT_EQUAL(std::string("Fake Street 123"), payload->getAddresses()[0].street);
			CPPUNIT_ASSERT_EQUAL(std::string("12345"), payload->getAddresses()[0].postalCode);
			CPPUNIT_ASSERT_EQUAL(std::string("USA"), payload->getAddresses()[0].country);
			CPPUNIT_ASSERT(payload->getAddresses()[0].isHome);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(payload->getAddressLabels().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Fake Street 123"), payload->getAddressLabels()[0].lines[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("12345 Any Town"), payload->getAddressLabels()[0].lines[1]);
			CPPUNIT_ASSERT_EQUAL(std::string("USA"), payload->getAddressLabels()[0].lines[2]);
			CPPUNIT_ASSERT(payload->getAddressLabels()[0].isHome);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getJIDs().size()));
			CPPUNIT_ASSERT_EQUAL(JID("alice@teaparty.lit"), payload->getJIDs()[0]);
			CPPUNIT_ASSERT_EQUAL(JID("alice@wonderland.lit"), payload->getJIDs()[1]);

			CPPUNIT_ASSERT_EQUAL(std::string("I once fell down a rabbit hole."), payload->getDescription());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(payload->getOrganizations().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Alice In Wonderland Inc."), payload->getOrganizations()[0].name);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(payload->getOrganizations()[0].units.size()));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(payload->getTitles().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Some Title"), payload->getTitles()[0]);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(payload->getRoles().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Main Character"), payload->getRoles()[0]);
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getURLs().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("http://wonderland.lit/~alice"), payload->getURLs()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("http://teaparty.lit/~alice2"), payload->getURLs()[1]);

			CPPUNIT_ASSERT_EQUAL(std::string("<MAILER xmlns=\"vcard-temp\">mutt</MAILER>"), payload->getUnknownContent());
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
