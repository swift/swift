/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include <Swiften/JID/JID.h>
#include <string>
#include <Swiften/Avatars/AvatarManagerImpl.h>
#include <Swiften/Avatars/CombinedAvatarProvider.h>
#include <Swiften/Avatars/VCardAvatarManager.h>
#include <Swiften/Avatars/OfflineAvatarManager.h>
#include <Swiften/Elements/VCardUpdate.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Avatars/AvatarMemoryStorage.h>
#include <Swiften/VCards/VCardMemoryStorage.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/Avatars/VCardUpdateAvatarManager.h>
#include <Swiften/StringCodecs/Hexify.h>

using namespace Swift;

class AvatarManagerImplTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(AvatarManagerImplTest);
		CPPUNIT_TEST(testGetSetAvatar);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			ownerJID = JID("owner@domain.com/theowner");
			stanzaChannel = boost::make_shared<DummyStanzaChannel>();
			iqRouter = boost::make_shared<IQRouter>(stanzaChannel.get());
			crypto = boost::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
			vcardStorage = boost::make_shared<VCardMemoryStorage>(crypto.get());
			vcardManager = boost::make_shared<VCardManager>(ownerJID, iqRouter.get(), vcardStorage.get());
			avatarStorage = boost::make_shared<AvatarMemoryStorage>();
			mucRegistry = boost::make_shared<DummyMUCRegistry>();
			avatarManager = boost::make_shared<AvatarManagerImpl>(vcardManager.get(), stanzaChannel.get(), avatarStorage.get(), crypto.get(), mucRegistry.get());
		}

		void testGetSetAvatar() {
			/* initially we have no knowledge of the user or their avatar */
			JID personJID("person@domain.com/theperson");
			ByteArray avatar = avatarManager->getAvatar(personJID.toBare());
			CPPUNIT_ASSERT(!avatar.size());

			/* notify the 'owner' JID that our avatar has changed */

			ByteArray fullAvatar = createByteArray("abcdefg");
			boost::shared_ptr<VCardUpdate> vcardUpdate = boost::make_shared<VCardUpdate>();
			vcardUpdate->setPhotoHash(Hexify::hexify(crypto->getSHA1Hash(fullAvatar)));
			boost::shared_ptr<Presence> presence = boost::make_shared<Presence>();
			presence->setTo(ownerJID);
			presence->setFrom(personJID);
			presence->setType(Presence::Available);
			presence->addPayload(vcardUpdate);
			stanzaChannel->onPresenceReceived(presence);

			/* reply to the avatar request with our new avatar */

			CPPUNIT_ASSERT_EQUAL(size_t(1), stanzaChannel->sentStanzas.size());
			boost::shared_ptr<IQ> request = boost::dynamic_pointer_cast<IQ>(stanzaChannel->sentStanzas[0]);
			stanzaChannel->sentStanzas.pop_back();
			CPPUNIT_ASSERT(!!request);
			boost::shared_ptr<VCard> vcard = request->getPayload<VCard>();
			CPPUNIT_ASSERT(!!vcard);

			boost::shared_ptr<IQ> reply = boost::make_shared<IQ>(IQ::Result);
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

			vcardUpdate = boost::make_shared<VCardUpdate>();
			presence = boost::make_shared<Presence>();
			presence->setTo(ownerJID);
			presence->setFrom(personJID);
			presence->setType(Presence::Available);
			presence->addPayload(vcardUpdate);
			stanzaChannel->onPresenceReceived(presence);

			/* reply to the avatar request with our EMPTY avatar */

			CPPUNIT_ASSERT_EQUAL(size_t(1), stanzaChannel->sentStanzas.size());
			request = boost::dynamic_pointer_cast<IQ>(stanzaChannel->sentStanzas[0]);
			stanzaChannel->sentStanzas.pop_back();
			CPPUNIT_ASSERT(!!request);
			vcard = request->getPayload<VCard>();
			CPPUNIT_ASSERT(!!vcard);

			ByteArray blankAvatar = createByteArray("");
			reply = boost::make_shared<IQ>(IQ::Result);
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
		boost::shared_ptr<DummyStanzaChannel> stanzaChannel;
		boost::shared_ptr<IQRouter> iqRouter;
		boost::shared_ptr<CryptoProvider> crypto;
		boost::shared_ptr<VCardMemoryStorage> vcardStorage;
		boost::shared_ptr<VCardManager> vcardManager;
		boost::shared_ptr<AvatarMemoryStorage> avatarStorage;
		boost::shared_ptr<DummyMUCRegistry> mucRegistry;
		boost::shared_ptr<AvatarManagerImpl> avatarManager;

};

CPPUNIT_TEST_SUITE_REGISTRATION(AvatarManagerImplTest);
