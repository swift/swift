/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include "Swiften/Elements/VCardUpdate.h"
#include "Swiften/Avatars/VCardUpdateAvatarManager.h"
#include "Swiften/Avatars/AvatarMemoryStorage.h"
#include "Swiften/VCards/VCardMemoryStorage.h"
#include "Swiften/VCards/VCardManager.h"
#include "Swiften/MUC/MUCRegistry.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/Hexify.h"

using namespace Swift;

class VCardUpdateAvatarManagerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(VCardUpdateAvatarManagerTest);
		CPPUNIT_TEST(testUpdate_NewHashNewVCardRequestsVCard);
		CPPUNIT_TEST(testUpdate_NewHashStoresAvatarAndEmitsNotificationOnVCardReceive);
		CPPUNIT_TEST(testUpdate_KnownHash);
		CPPUNIT_TEST(testUpdate_KnownHashFromDifferentUserDoesNotRequestVCardButTriggersNotification);
		CPPUNIT_TEST(testVCardWithEmptyPhoto);
		CPPUNIT_TEST(testStanzaChannelReset);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			ownJID = JID("foo@fum.com/bum");
			stanzaChannel = new DummyStanzaChannel();
			stanzaChannel->setAvailable(true);
			iqRouter = new IQRouter(stanzaChannel);
			mucRegistry = new DummyMUCRegistry();
			avatarStorage = new AvatarMemoryStorage();
			vcardStorage = new VCardMemoryStorage();
			vcardManager = new VCardManager(ownJID, iqRouter, vcardStorage);
			avatar1 = ByteArray("abcdefg");
			avatar1Hash = Hexify::hexify(SHA1::getHash(avatar1));
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
			std::auto_ptr<VCardUpdateAvatarManager> testling = createManager();
			stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(0, user1.toBare(), IQ::Get));
		}

		void testUpdate_NewHashStoresAvatarAndEmitsNotificationOnVCardReceive() {
			std::auto_ptr<VCardUpdateAvatarManager> testling = createManager();
			stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
			stanzaChannel->onIQReceived(createVCardResult(avatar1));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(avatar1Hash, changes[0].second);
			CPPUNIT_ASSERT(avatarStorage->hasAvatar(avatar1Hash));
			CPPUNIT_ASSERT_EQUAL(avatar1, avatarStorage->getAvatar(avatar1Hash));
		}

		void testUpdate_KnownHash() {
			std::auto_ptr<VCardUpdateAvatarManager> testling = createManager();
			stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
			stanzaChannel->onIQReceived(createVCardResult(avatar1));
			changes.clear();
			stanzaChannel->sentStanzas.clear();

			stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
		}

		void testUpdate_KnownHashFromDifferentUserDoesNotRequestVCardButTriggersNotification() {
			std::auto_ptr<VCardUpdateAvatarManager> testling = createManager();
			stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
			stanzaChannel->onIQReceived(createVCardResult(avatar1));
			changes.clear();
			stanzaChannel->sentStanzas.clear();

			stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user2, avatar1Hash));

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user2.toBare(), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(avatar1Hash, changes[0].second);
		}

		void testVCardWithEmptyPhoto() {
			std::auto_ptr<VCardUpdateAvatarManager> testling = createManager();
			vcardManager->requestVCard(JID("foo@bar.com"));
			stanzaChannel->onIQReceived(createVCardResult(ByteArray()));
			
			CPPUNIT_ASSERT(!avatarStorage->hasAvatar(Hexify::hexify(SHA1::getHash(ByteArray()))));
			CPPUNIT_ASSERT_EQUAL(boost::filesystem::path(), testling->getAvatarPath(JID("foo@bar.com")));
		}

		void testStanzaChannelReset() {
			std::auto_ptr<VCardUpdateAvatarManager> testling = createManager();
			stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));
			stanzaChannel->onIQReceived(createVCardResult(avatar1));
			changes.clear();
			stanzaChannel->sentStanzas.clear();

			stanzaChannel->setAvailable(false);
			stanzaChannel->setAvailable(true);
			stanzaChannel->onPresenceReceived(createPresenceWithPhotoHash(user1, avatar1Hash));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(avatar1Hash, changes[0].second);
		}

	private:
		std::auto_ptr<VCardUpdateAvatarManager> createManager() {
			std::auto_ptr<VCardUpdateAvatarManager> result(new VCardUpdateAvatarManager(vcardManager, stanzaChannel, avatarStorage, mucRegistry));
			result->onAvatarChanged.connect(boost::bind(&VCardUpdateAvatarManagerTest::handleAvatarChanged, this, _1, _2));
			return result;
		}

		boost::shared_ptr<Presence> createPresenceWithPhotoHash(const JID& jid, const String& hash) {
			boost::shared_ptr<Presence> presence(new Presence());
			presence->setFrom(jid);
			presence->addPayload(boost::shared_ptr<VCardUpdate>(new VCardUpdate(hash)));
			return presence;
		}

		IQ::ref createVCardResult(const ByteArray& avatar) {
			VCard::ref vcard(new VCard());
			if (!avatar.isEmpty()) {
				vcard->setPhoto(avatar);
			}
			return IQ::createResult(JID("baz@fum.com"), stanzaChannel->sentStanzas[0]->getID(), vcard);
		}

		void handleAvatarChanged(const JID& jid, const String& hash) {
			changes.push_back(std::pair<JID,String>(jid, hash));
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
		String avatar1Hash;
		std::vector<std::pair<JID,String> > changes;
		JID user1;
		JID user2;
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardUpdateAvatarManagerTest);
