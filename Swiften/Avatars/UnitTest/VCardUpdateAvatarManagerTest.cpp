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
#include <Swiften/Avatars/VCardUpdateAvatarManager.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Elements/VCardUpdate.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>

using namespace Swift;

class VCardUpdateAvatarManagerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(VCardUpdateAvatarManagerTest);
        CPPUNIT_TEST(testUpdate_NewHashNewVCardRequestsVCard);
        CPPUNIT_TEST(testUpdate_NewHashStoresAvatarAndEmitsNotificationOnVCardReceive);
        CPPUNIT_TEST(testUpdate_KnownHash);
        CPPUNIT_TEST(testUpdate_KnownHashFromDifferentUserDoesNotRequestVCardButTriggersNotification);
        CPPUNIT_TEST(testVCardWithEmptyPhoto);
        CPPUNIT_TEST(testStanzaChannelReset_ClearsHash);
        CPPUNIT_TEST(testStanzaChannelReset_ReceiveHashAfterResetUpdatesHash);
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

        void testUpdate_NewHashNewVCardRequestsVCard() {
            std::shared_ptr<VCardUpdateAvatarManager> testling = createManager();
            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
            CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(0, user1.toBare(), IQ::Get));
        }

        void testUpdate_NewHashStoresAvatarAndEmitsNotificationOnVCardReceive() {
            std::shared_ptr<VCardUpdateAvatarManager> testling = createManager();
            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
            stanzaChannel->onIQReceived(createVCardResult(avatar1));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[0]);
            boost::optional<std::string> hash = testling->getAvatarHash(user1.toBare());
            CPPUNIT_ASSERT(hash);
            CPPUNIT_ASSERT_EQUAL(avatar1Hash, *hash);
            CPPUNIT_ASSERT(avatarStorage->hasAvatar(avatar1Hash));
            CPPUNIT_ASSERT_EQUAL(avatar1, avatarStorage->getAvatar(avatar1Hash));
        }

        void testUpdate_KnownHash() {
            std::shared_ptr<VCardUpdateAvatarManager> testling = createManager();
            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
            stanzaChannel->onIQReceived(createVCardResult(avatar1));
            changes.clear();
            stanzaChannel->sentStanzas.clear();

            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
        }

        void testUpdate_KnownHashFromDifferentUserDoesNotRequestVCardButTriggersNotification() {
            std::shared_ptr<VCardUpdateAvatarManager> testling = createManager();
            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
            stanzaChannel->onIQReceived(createVCardResult(avatar1));
            changes.clear();
            stanzaChannel->sentStanzas.clear();

            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user2, avatar1Hash));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user2.toBare(), changes[0]);
            boost::optional<std::string> hash = testling->getAvatarHash(user2.toBare());
            CPPUNIT_ASSERT(hash);
            CPPUNIT_ASSERT_EQUAL(avatar1Hash, *hash);
        }

        void testVCardWithEmptyPhoto() {
            std::shared_ptr<VCardUpdateAvatarManager> testling = createManager();
            vcardManager->requestVCard(JID("foo@bar.com"));
            stanzaChannel->onIQReceived(createVCardResult(ByteArray()));

            CPPUNIT_ASSERT(!avatarStorage->hasAvatar(Hexify::hexify(crypto->getSHA1Hash(ByteArray()))));
            boost::optional<std::string> hash = testling->getAvatarHash(JID("foo@bar.com"));
            CPPUNIT_ASSERT(hash);
            CPPUNIT_ASSERT_EQUAL(std::string(), *hash);
        }

        void testStanzaChannelReset_ClearsHash() {
            std::shared_ptr<VCardUpdateAvatarManager> testling = createManager();
            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
            stanzaChannel->onIQReceived(createVCardResult(avatar1));
            changes.clear();
            stanzaChannel->sentStanzas.clear();

            stanzaChannel->setAvailable(false);
            stanzaChannel->setAvailable(true);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[0]);
            boost::optional<std::string> hash = testling->getAvatarHash(user1.toBare());
            CPPUNIT_ASSERT(!hash);
            //CPPUNIT_ASSERT_EQUAL(std::string(""), *hash);
        }

        void testStanzaChannelReset_ReceiveHashAfterResetUpdatesHash() {
            std::shared_ptr<VCardUpdateAvatarManager> testling = createManager();
            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
            stanzaChannel->onIQReceived(createVCardResult(avatar1));
            changes.clear();
            stanzaChannel->sentStanzas.clear();

            stanzaChannel->setAvailable(false);
            stanzaChannel->setAvailable(true);
            stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(changes.size()));
            CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[1]);
            boost::optional<std::string> hash = testling->getAvatarHash(user1.toBare());
            CPPUNIT_ASSERT(hash);
            CPPUNIT_ASSERT_EQUAL(avatar1Hash, *hash);
        }

    private:
        std::shared_ptr<VCardUpdateAvatarManager> createManager() {
            std::shared_ptr<VCardUpdateAvatarManager> result(new VCardUpdateAvatarManager(vcardManager, stanzaChannel, avatarStorage, crypto.get(), mucRegistry));
            result->onAvatarChanged.connect(boost::bind(&VCardUpdateAvatarManagerTest::handleAvatarChanged, this, _1));
            return result;
        }

        std::shared_ptr<Presence> createPresenceWithPhotoHash(const JID& jid, const std::string& hash) {
            std::shared_ptr<Presence> presence(new Presence());
            presence->setFrom(jid);
            presence->addPayload(std::make_shared<VCardUpdate>(hash));
            return presence;
        }

        IQ::ref createVCardResult(const ByteArray& avatar) {
            VCard::ref vcard(new VCard());
            if (!avatar.empty()) {
                vcard->setPhoto(avatar);
            }
            return IQ::createResult(JID("baz@fum.com"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID(), vcard);
        }

        void handleAvatarChanged(const JID& jid) {
            changes.push_back(jid);
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

CPPUNIT_TEST_SUITE_REGISTRATION(VCardUpdateAvatarManagerTest);
