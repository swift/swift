/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/date_time/posix_time/posix_time.hpp>

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Parser/PayloadParsers/VCardParser.h>

using namespace Swift;

class VCardParserTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(VCardParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST(testParse_Photo);
        CPPUNIT_TEST(testParse_NewlinedPhoto);
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

            std::shared_ptr<VCard> payload = std::dynamic_pointer_cast<VCard>(parser.getPayload());
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

        void testParse_NewlinedPhoto() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                "<vCard xmlns='vcard-temp'>"
                    "<PHOTO>"
                        "<TYPE>image/jpeg</TYPE>"
                        "<BINVAL>"
                            "dTsETKSAskgu2/BqVO+ogcu3DJy4QATGJqpsa6znWwNGiLnVElVVB6PtS+mTiHUXsrOlKvRjtvzV\n"
                            "VDknNaRF58Elmu5EC6VoCllBEEB/lFf0emYn2gkp0X1khNi75dl+rOj95Ar6XuwLh+ZoSStqwOWj\n"
                            "pIpxmZmVw7E69qr0FY0oI3zcaxXwzHw7Lx9Qf4sH7ufQvIN88ga+hwp8MiXevh3Ac8pN00kgINlq\n"
                            "9AY/bYJL418Y/6wWsJbgmrJ/N78wSMpC7VVszLBZVv8uFnupubyi8Ophd/1wIWWzPPwAbBhepWVb\n"
                            "1oPiFEBT5MNKCMTPEi0npXtedVz0HQbbPNIVwmo="
                        "</BINVAL>"
                    "</PHOTO>"
                "</vCard>"));

            VCard* payload = dynamic_cast<VCard*>(parser.getPayload().get());
            CPPUNIT_ASSERT_EQUAL(std::string("image/jpeg"), payload->getPhotoType());
            CPPUNIT_ASSERT_EQUAL(createByteArray("\x75\x3B\x04\x4C\xA4\x80\xB2\x48\x2E\xDB\xF0\x6A\x54\xEF\xA8\x81\xCB\xB7\x0C\x9C\xB8\x40\x04\xC6\x26\xAA\x6C\x6B\xAC\xE7\x5B\x03\x46\x88\xB9\xD5\x12\x55\x55\x07\xA3\xED\x4B\xE9\x93\x88\x75\x17\xB2\xB3\xA5\x2A\xF4\x63\xB6\xFC\xD5\x54\x39\x27\x35\xA4\x45\xE7\xC1\x25\x9A\xEE\x44\x0B\xA5\x68\x0A\x59\x41\x10\x40\x7F\x94\x57\xF4\x7A\x66\x27\xDA\x09\x29\xD1\x7D\x64\x84\xD8\xBB\xE5\xD9\x7E\xAC\xE8\xFD\xE4\x0A\xFA\x5E\xEC\x0B\x87\xE6\x68\x49\x2B\x6A\xC0\xE5\xA3\xA4\x8A\x71\x99\x99\x95\xC3\xB1\x3A\xF6\xAA\xF4\x15\x8D\x28\x23\x7C\xDC\x6B\x15\xF0\xCC\x7C\x3B\x2F\x1F\x50\x7F\x8B\x07\xEE\xE7\xD0\xBC\x83\x7C\xF2\x06\xBE\x87\x0A\x7C\x32\x25\xDE\xBE\x1D\xC0\x73\xCA\x4D\xD3\x49\x20\x20\xD9\x6A\xF4\x06\x3F\x6D\x82\x4B\xE3\x5F\x18\xFF\xAC\x16\xB0\x96\xE0\x9A\xB2\x7F\x37\xBF\x30\x48\xCA\x42\xED\x55\x6C\xCC\xB0\x59\x56\xFF\x2E\x16\x7B\xA9\xB9\xBC\xA2\xF0\xEA\x61\x77\xFD\x70\x21\x65\xB3\x3C\xFC\x00\x6C\x18\x5E\xA5\x65\x5B\xD6\x83\xE2\x14\x40\x53\xE4\xC3\x4A\x08\xC4\xCF\x12\x2D\x27\xA5\x7B\x5E\x75\x5C\xF4\x1D\x06\xDB\x3C\xD2\x15\xC2\x6A", 257), payload->getPhoto());
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
