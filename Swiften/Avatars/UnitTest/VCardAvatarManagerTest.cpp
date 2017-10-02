/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Avatars/AvatarMemoryStorage.h>
#include <Swiften/Avatars/VCardAvatarManager.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>

using namespace Swift;

class VCardAvatarManagerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(VCardAvatarManagerTest);
        CPPUNIT_TEST(testGetAvatarHashKnownAvatar);
        CPPUNIT_TEST(testGetAvatarHashEmptyAvatar);
        CPPUNIT_TEST(testGetAvatarHashUnknownAvatarKnownVCardStoresAvatar);
        CPPUNIT_TEST(testGetAvatarHashUnknownAvatarUnknownVCard);
        CPPUNIT_TEST(testVCardUpdateTriggersUpdate);
        CPPUNIT_TEST(testGetAvatarHashKnownAvatarUnknownVCard);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
            ownJID = JID("foo@fum.com/bum");
            stanzaChannel = new DummyStanzaChannel();
            stanzaChannel->setAvailable(true);
            iqRouter = new IQRouter(stanzaChannel);
            mucRegistry = new DummyMUCRegistry();
            avatarStorage = new AvatarMemoryStorage();
            vcardStorage = new VCardMemoryStorage(crypto.get());
            vcardManager = new VCardManager(ownJID, iqRouter, vcardStorage);
            avatar1 = createByteArray("abcdefg");
            avatar1Hash = Hexify::hexify(crypto->getSHA1Hash(avatar1));
            user1 = JID("user1@bar.com/bla");
            user2 = JID("user2@foo.com/baz");
        }

        void tearDown() {
            delete vcardManager;
            delete vcardStorage;
            delete avatarStorage;
            delete mucRegistry;
            delete iqRouter;
            delete stanzaChannel;
        }

        void testGetAvatarHashKnownAvatar() {
            std::shared_ptr<VCardAvatarManager> testling = createManager();
            storeVCardWithPhoto(user1.toBare(), avatar1);
            avatarStorage->addAvatar(avatar1Hash, avatar1);

            boost::optional<std::string> result = testling->getAvatarHash(user1);

            CPPUNIT_ASSERT(result);
            CPPUNIT_ASSERT_EQUAL(avatar1Hash, *result);
        }

        void testGetAvatarHashEmptyAvatar() {
            std::shared_ptr<VCardAvatarManager> testling = createManager();
            storeEmptyVCard(user1.toBare());

            boost::optional<std::string> result = testling->getAvatarHash(user1);

            CPPUNIT_ASSERT(result);
            CPPUNIT_ASSERT_EQUAL(std::string(), *result);
        }

        void testGetAvatarHashUnknownAvatarKnownVCardStoresAvatar() {
            std::shared_ptr<VCardAvatarManager> testling = createManager();
            storeVCardWithPhoto(user1.toBare(), avatar1);

            boost::optional<std::string> result = testling->getAvatarHash(user1);

            CPPUNIT_ASSERT(result);
            CPPUNIT_ASSERT_EQUAL(avatar1Hash, *result);
            CPPUNIT_ASSERT(avatarStorage->hasAvatar(avatar1Hash));
            CPPUNIT_ASSERT_EQUAL(avatar1, avatarStorage->getAvatar(avatar1Hash));
        }

        void testGetAvatarHashUnknownAvatarUnknownVCard() {
            std::shared_ptr<VCardAvatarManager> testling = createManager();

            boost::optional<std::string> result = testling->getAvatarHash(user1);

            CPPUNIT_ASSERT(result);
            CPPUNIT_ASSERT_EQUAL(std::string(), *result);
        }

        void testGetAvatarHashKnownAvatarUnknownVCard() {
            std::shared_ptr<VCardAvatarManager> testling = createManager();

            avatarStorage->setAvatarForJID(user1, avatar1Hash);

            boost::optional<std::string> result = testling->getAvatarHash(user1);

            CPPUNIT_ASSERT(result);
            CPPUNIT_ASSERT_EQUAL(std::string(), *result);
        }


        void testVCardUpdateTriggersUpdate() {
            std::shared_ptr<VCardAvatarManager> testling = createManager();
            vcardManager->requestVCard(user1);
            sendVCardResult();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
        }

    private:
        std::shared_ptr<VCardAvatarManager> createManager() {
            std::shared_ptr<VCardAvatarManager> result(new VCardAvatarManager(vcardManager, avatarStorage, crypto.get(), mucRegistry));
            result->onAvatarChanged.connect(boost::bind(&VCardAvatarManagerTest::handleAvatarChanged, this, _1));
            return result;
        }

        void storeVCardWithPhoto(const JID& jid, const ByteArray& avatar) {
            VCard::ref vcard(new VCard());
            vcard->setPhoto(avatar);
            vcardStorage->setVCard(jid, vcard);
        }

        void storeEmptyVCard(const JID& jid) {
            VCard::ref vcard(new VCard());
            vcardStorage->setVCard(jid, vcard);
        }

        void handleAvatarChanged(const JID& jid) {
            changes.push_back(jid);
        }

        void sendVCardResult() {
            VCard::ref vcard(new VCard());
            vcard->setFullName("Foo Bar");
            stanzaChannel->onIQReceived(IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID(), vcard));
        }

    private:
        struct DummyMUCRegistry : public MUCRegistry {
            bool isMUC(const JID& jid) const { return std::find(mucs_.begin(), mucs_.end(), jid) != mucs_.end(); }
            std::vector<JID> mucs_;
        };

        JID ownJID;
        DummyStanzaChannel* stanzaChannel;
        IQRouter* iqRouter;
        DummyMUCRegistry* mucRegistry;
        AvatarMemoryStorage* avatarStorage;
        VCardManager* vcardManager;
        VCardMemoryStorage* vcardStorage;
        ByteArray avatar1;
        std::string avatar1Hash;
        std::vector<JID> changes;
        JID user1;
        JID user2;
        std::shared_ptr<CryptoProvider> crypto;
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardAvatarManagerTest);
