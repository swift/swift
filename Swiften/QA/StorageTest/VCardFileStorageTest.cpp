/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>
#include <Swiften/VCards/VCardFileStorage.h>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>

using namespace Swift;

class VCardFileStorageTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(VCardFileStorageTest);
        CPPUNIT_TEST(testSetVCard);
        // Temporarily disabling this, because it generates error messages on console. Need to figure
        // out something for not showing error messages during tests.
        //CPPUNIT_TEST(testSetVCard_LargeFilename);
        CPPUNIT_TEST(testGetVCard);
        CPPUNIT_TEST(testGetVCard_FileDoesNotExist);
        //CPPUNIT_TEST(testGetVCard_LargeFilename);
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
            std::shared_ptr<VCardFileStorage> testling(createTestling());
            VCard::ref vcard(new VCard());
            vcard->setFullName("Alice In Wonderland");

            testling->setVCard(JID("alice@wonderland.lit/TeaRoom"), vcard);

            boost::filesystem::path vcardFile(vcardsPath / "alice@wonderland.lit%2fTeaRoom.xml");
            CPPUNIT_ASSERT(boost::filesystem::exists(vcardFile));
            ByteArray data;
            data.readFromFile(vcardFile);
            CPPUNIT_ASSERT(boost::starts_with(data.toString(), "<vCard xmlns=\"vcard-temp\">"));
        }

        void testSetVCard_LargeFilename() {
            std::unique_ptr<VCardFileStorage> testling(createTestling());
            VCard::ref vcard(new VCard());
            vcard->setFullName("Alice In Wonderland");

            std::ostringstream s;
            for (int i = 0; i < 1000; ++i) {
                s << "_";
            }

            JID jid("alice@wonderland.lit/" + s.str());
            testling->setVCard(jid, vcard);

            // Just check whether we don't crash
        }

        void testGetVCard() {
            std::shared_ptr<VCardFileStorage> testling(createTestling());
            VCard::ref vcard(new VCard());
            vcard->setFullName("Alice In Wonderland");
            testling->setVCard(JID("alice@wonderland.lit"), vcard);

            VCard::ref result = testling->getVCard(JID("alice@wonderland.lit"));
            CPPUNIT_ASSERT_EQUAL(std::string("Alice In Wonderland"), result->getFullName());
        }

        void testGetVCard_LargeFilename() {
            std::unique_ptr<VCardFileStorage> testling(createTestling());
            VCard::ref vcard(new VCard());
            vcard->setFullName("Alice In Wonderland");

            std::ostringstream s;
            for (int i = 0; i < 1000; ++i) {
                s << "_";
            }
            JID jid("alice@wonderland.lit/" + s.str());

            VCard::ref result = testling->getVCard(jid);

            // Just check that we don't have an exception
        }

        void testGetVCard_FileDoesNotExist() {
            std::shared_ptr<VCardFileStorage> testling(createTestling());
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
