/*
 * Copyright (c) 2014-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Avatars/AvatarManagerImpl.h>
#include <Swiften/Avatars/AvatarMemoryStorage.h>
#include <Swiften/Avatars/CombinedAvatarProvider.h>
#include <Swiften/Avatars/OfflineAvatarManager.h>
#include <Swiften/Avatars/VCardAvatarManager.h>
#include <Swiften/Avatars/VCardUpdateAvatarManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Elements/VCardUpdate.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>

using namespace Swift;

class AvatarManagerImplTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(AvatarManagerImplTest);
        CPPUNIT_TEST(testGetSetAvatar);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            ownerJID = JID("owner@domain.com/theowner");
            stanzaChannel = std::make_shared<DummyStanzaChannel>();
            iqRouter = std::make_shared<IQRouter>(stanzaChannel.get());
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
            vcardStorage = std::make_shared<VCardMemoryStorage>(crypto.get());
            vcardManager = std::make_shared<VCardManager>(ownerJID, iqRouter.get(), vcardStorage.get());
            avatarStorage = std::make_shared<AvatarMemoryStorage>();
            mucRegistry = std::make_shared<DummyMUCRegistry>();
            avatarManager = std::make_shared<AvatarManagerImpl>(vcardManager.get(), stanzaChannel.get(), avatarStorage.get(), crypto.get(), mucRegistry.get());
        }

        void testGetSetAvatar() {
            /* initially we have no knowledge of the user or their avatar */
            JID personJID("person@domain.com/theperson");
            ByteArray avatar = avatarManager->getAvatar(personJID.toBare());
            CPPUNIT_ASSERT(!avatar.size());

            /* notify the 'owner' JID that our avatar has changed */

            ByteArray fullAvatar = createByteArray("abcdefg");
            std::shared_ptr<VCardUpdate> vcardUpdate = std::make_shared<VCardUpdate>();
            vcardUpdate->setPhotoHash(Hexify::hexify(crypto->getSHA1Hash(fullAvatar)));
            std::shared_ptr<Presence> presence = std::make_shared<Presence>();
            presence->setTo(ownerJID);
            presence->setFrom(personJID);
            presence->setType(Presence::Available);
            presence->addPayload(vcardUpdate);
            stanzaChannel->onPresenceReceived(presence);

            /* reply to the avatar request with our new avatar */

            CPPUNIT_ASSERT_EQUAL(size_t(1), stanzaChannel->sentStanzas.size());
            std::shared_ptr<IQ> request = std::dynamic_pointer_cast<IQ>(stanzaChannel->sentStanzas[0]);
            stanzaChannel->sentStanzas.pop_back();
            CPPUNIT_ASSERT(!!request);
            std::shared_ptr<VCard> vcard = request->getPayload<VCard>();
            CPPUNIT_ASSERT(!!vcard);

            std::shared_ptr<IQ> reply = std::make_shared<IQ>(IQ::Result);
            reply->setTo(request->getFrom());
            reply->setFrom(request->getTo());
            reply->setID(request->getID());
            vcard->setPhoto(fullAvatar);
            reply->addPayload(vcard);
            stanzaChannel->onIQReceived(reply);

            /* check hash through avatarManager that it received the correct photo */

            ByteArray reportedAvatar = avatarManager->getAvatar(personJID.toBare());
            CPPUNIT_ASSERT_EQUAL(byteArrayToString(fullAvatar), byteArrayToString(reportedAvatar));

            /* send new presence to notify of blank avatar */

            vcardUpdate = std::make_shared<VCardUpdate>();
            vcardUpdate->setPhotoHash("da39a3ee5e6b4b0d3255bfef95601890afd80709");
            presence = std::make_shared<Presence>();
            presence->setTo(ownerJID);
            presence->setFrom(personJID);
            presence->setType(Presence::Available);
            presence->addPayload(vcardUpdate);
            stanzaChannel->onPresenceReceived(presence);

            /* reply to the avatar request with our EMPTY avatar */

            CPPUNIT_ASSERT_EQUAL(size_t(1), stanzaChannel->sentStanzas.size());
            request = std::dynamic_pointer_cast<IQ>(stanzaChannel->sentStanzas[0]);
            stanzaChannel->sentStanzas.pop_back();
            CPPUNIT_ASSERT(!!request);
            vcard = request->getPayload<VCard>();
            CPPUNIT_ASSERT(!!vcard);

            ByteArray blankAvatar = createByteArray("");
            reply = std::make_shared<IQ>(IQ::Result);
            reply->setTo(request->getFrom());
            reply->setFrom(request->getTo());
            reply->setID(request->getID());
            vcard->setPhoto(blankAvatar);
            reply->addPayload(vcard);
            stanzaChannel->onIQReceived(reply);

            /* check hash through avatarManager that it received the correct photo */

            reportedAvatar = avatarManager->getAvatar(personJID.toBare());
            CPPUNIT_ASSERT_EQUAL(byteArrayToString(blankAvatar), byteArrayToString(reportedAvatar));
        }

        struct DummyMUCRegistry : public MUCRegistry {
            bool isMUC(const JID& jid) const { return std::find(mucs_.begin(), mucs_.end(), jid) != mucs_.end(); }
            std::vector<JID> mucs_;
        };

    private:

        JID ownerJID;
        std::shared_ptr<DummyStanzaChannel> stanzaChannel;
        std::shared_ptr<IQRouter> iqRouter;
        std::shared_ptr<CryptoProvider> crypto;
        std::shared_ptr<VCardMemoryStorage> vcardStorage;
        std::shared_ptr<VCardManager> vcardManager;
        std::shared_ptr<AvatarMemoryStorage> avatarStorage;
        std::shared_ptr<DummyMUCRegistry> mucRegistry;
        std::shared_ptr<AvatarManagerImpl> avatarManager;

};

CPPUNIT_TEST_SUITE_REGISTRATION(AvatarManagerImplTest);
