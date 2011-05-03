/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
			boost::shared_ptr<VCard> vcard(new VCard());
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
			vcard->addUnknownContent("<BDAY>1234</BDAY><MAILER>mutt</MAILER>");

			VCard::EMailAddress address1;
			address1.address = "alice@wonderland.lit";
			address1.isHome = true;
			address1.isPreferred = true;
			address1.isInternet = true;
			vcard->addEMailAddress(address1);

			VCard::EMailAddress address2;
			address2.address = "alice@teaparty.lit";
			address2.isWork = true;
			address2.isX400 = true;
			vcard->addEMailAddress(address2);

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
					"<BDAY>1234</BDAY>"
					"<MAILER>mutt</MAILER>"
				"</vCard>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(vcard));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardSerializerTest);
