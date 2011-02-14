/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/algorithm/string.hpp>

#include "Swiften/VCards/VCardFileStorage.h"
#include "Swiften/JID/JID.h"
#include "SwifTools/Application/PlatformApplicationPathProvider.h"
#include "Swiften/Elements/VCard.h"

using namespace Swift;

class VCardFileStorageTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(VCardFileStorageTest);
		CPPUNIT_TEST(testSetVCard);
		CPPUNIT_TEST(testGetVCard);
		CPPUNIT_TEST(testGetVCard_FileDoesNotExist);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			pathProvider = new PlatformApplicationPathProvider("VCardStorageTest");
			vcardsPath = pathProvider->getExecutableDir() / "vcards";
			boost::filesystem::remove_all(vcardsPath);
		}

		void tearDown() {
			delete pathProvider;
			boost::filesystem::remove_all(vcardsPath);
		}

		void testSetVCard() {
			std::auto_ptr<VCardFileStorage> testling(createTestling());
			VCard::ref vcard(new VCard());
			vcard->setFullName("Alice In Wonderland");

			testling->setVCard(JID("alice@wonderland.lit/TeaRoom"), vcard);

			boost::filesystem::path vcardFile(vcardsPath / "alice@wonderland.lit%2fTeaRoom.xml");
			CPPUNIT_ASSERT(boost::filesystem::exists(vcardFile));
			ByteArray data;
			data.readFromFile(vcardFile.string());
			CPPUNIT_ASSERT(boost::starts_with(data.toString(), "<vCard xmlns=\"vcard-temp\">"));
		}

		void testGetVCard() {
			std::auto_ptr<VCardFileStorage> testling(createTestling());
			VCard::ref vcard(new VCard());
			vcard->setFullName("Alice In Wonderland");
			testling->setVCard(JID("alice@wonderland.lit"), vcard);

			VCard::ref result = testling->getVCard(JID("alice@wonderland.lit"));
			CPPUNIT_ASSERT_EQUAL(std::string("Alice In Wonderland"), result->getFullName());
		}

		void testGetVCard_FileDoesNotExist() {
			std::auto_ptr<VCardFileStorage> testling(createTestling());
			VCard::ref result = testling->getVCard(JID("alice@wonderland.lit"));
			CPPUNIT_ASSERT(!result);
		}

	private:
		VCardFileStorage* createTestling() {
			return new VCardFileStorage(vcardsPath);
		}

		PlatformApplicationPathProvider* pathProvider;
		boost::filesystem::path vcardsPath;
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardFileStorageTest);
