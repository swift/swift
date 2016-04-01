/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/VCardSerializer.h>

using namespace Swift;

class VCardSerializerTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(VCardSerializerTest);
        CPPUNIT_TEST(testSerialize);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerialize() {
            VCardSerializer testling;
            std::shared_ptr<VCard> vcard(new VCard());
            vcard->setVersion("2.0");
            vcard->setFullName("Alice In Wonderland");
            vcard->setPrefix("Mrs");
            vcard->setGivenName("Alice");
            vcard->setMiddleName("In");
            vcard->setFamilyName("Wonderland");
            vcard->setSuffix("PhD");
            vcard->setNickname("DreamGirl");
            vcard->setPhoto(createByteArray("abcdef"));
            vcard->setPhotoType("image/png");
            vcard->setBirthday(boost::posix_time::ptime(boost::gregorian::date(1865, 5, 4)));
            vcard->addUnknownContent("<MAILER>mutt</MAILER>");

            VCard::EMailAddress emailAddress1;
            emailAddress1.address = "alice@wonderland.lit";
            emailAddress1.isHome = true;
            emailAddress1.isPreferred = true;
            emailAddress1.isInternet = true;
            vcard->addEMailAddress(emailAddress1);

            VCard::EMailAddress address2;
            address2.address = "alice@teaparty.lit";
            address2.isWork = true;
            address2.isX400 = true;
            vcard->addEMailAddress(address2);

            VCard::Telephone telephone1;
            telephone1.number = "555-6273";
            telephone1.isHome = true;
            telephone1.isVoice = true;
            vcard->addTelephone(telephone1);

            VCard::Address address1;
            address1.locality = "Any Town";
            address1.street = "Fake Street 123";
            address1.postalCode = "12345";
            address1.country = "USA";
            address1.isHome = true;
            vcard->addAddress(address1);

            VCard::AddressLabel label1;
            label1.lines.push_back("Fake Street 123");
            label1.lines.push_back("12345 Any Town");
            label1.lines.push_back("USA");
            label1.isHome = true;
            vcard->addAddressLabel(label1);

            vcard->addJID(JID("alice@teaparty.lit"));
            vcard->addJID(JID("alice@wonderland.lit"));

            vcard->setDescription("I once fell down a rabbit hole.");

            VCard::Organization org1;
            org1.name = "Alice In Wonderland Inc.";
            vcard->addOrganization(org1);

            vcard->addTitle("Some Title");
            vcard->addRole("Main Character");
            vcard->addURL("http://wonderland.lit/~alice");
            vcard->addURL("http://teaparty.lit/~alice2");

            std::string expectedResult =
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
                    "<PHOTO>"
                        "<TYPE>image/png</TYPE>"
                        "<BINVAL>YWJjZGVm</BINVAL>"
                    "</PHOTO>"
                    "<BDAY>1865-05-04T00:00:00Z</BDAY>"
                    "<TEL>"
                        "<NUMBER>555-6273</NUMBER>"
                        "<HOME/>"
                        "<VOICE/>"
                    "</TEL>"
                    "<ADR>"
                        "<STREET>Fake Street 123</STREET>"
                        "<LOCALITY>Any Town</LOCALITY>"
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
                "</vCard>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(vcard));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardSerializerTest);
