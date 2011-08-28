/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include <Swiften/Elements/VCard.h>
#include <Swiften/Avatars/VCardAvatarManager.h>
#include <Swiften/Avatars/AvatarMemoryStorage.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardStorage.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/StringCodecs/Hexify.h>

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
		class TestVCardStorage : public VCardStorage {
			public:
				virtual VCard::ref getVCard(const JID& jid) const {
					VCardMap::const_iterator i = vcards.find(jid);
					if (i != vcards.end()) {
						return i->second;
					}
					else {
						return VCard::ref();
					}
				}

				virtual void setVCard(const JID& jid, VCard::ref v) {
					vcards[jid] = v;
				}

				std::string getPhotoHash(const JID& jid) const {
					if (photoHashes.find(jid) != photoHashes.end()) {
						return photoHashes.find(jid)->second;
					}
					VCard::ref vCard = getVCard(jid);
					if (vCard && !vCard->getPhoto().empty()) {
						return Hexify::hexify(SHA1::getHash(vCard->getPhoto()));
					}
					else {
						return "";
					}
				}

				std::map<JID, std::string> photoHashes;
				
			private:
				typedef std::map<JID, VCard::ref> VCardMap;
				VCardMap vcards;
		};

		void setUp() {
			ownJID = JID("foo@fum.com/bum");
			stanzaChannel = new DummyStanzaChannel();
			stanzaChannel->setAvailable(true);
			iqRouter = new IQRouter(stanzaChannel);
			mucRegistry = new DummyMUCRegistry();
			avatarStorage = new AvatarMemoryStorage();
			vcardStorage = new TestVCardStorage();
			vcardManager = new VCardManager(ownJID, iqRouter, vcardStorage);
			avatar1 = createByteArray("abcdefg");
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

		void testGetAvatarHashKnownAvatar() {
			boost::shared_ptr<VCardAvatarManager> testling = createManager();
			storeVCardWithPhoto(user1.toBare(), avatar1);
			avatarStorage->addAvatar(avatar1Hash, avatar1);

			std::string result = testling->getAvatarHash(user1);

			CPPUNIT_ASSERT_EQUAL(avatar1Hash, result);
		}

		void testGetAvatarHashEmptyAvatar() {
			boost::shared_ptr<VCardAvatarManager> testling = createManager();
			storeEmptyVCard(user1.toBare());

			std::string result = testling->getAvatarHash(user1);

			CPPUNIT_ASSERT_EQUAL(std::string(), result);
		}

		void testGetAvatarHashUnknownAvatarKnownVCardStoresAvatar() {
			boost::shared_ptr<VCardAvatarManager> testling = createManager();
			storeVCardWithPhoto(user1.toBare(), avatar1);

			std::string result = testling->getAvatarHash(user1);

			CPPUNIT_ASSERT_EQUAL(avatar1Hash, result);
			CPPUNIT_ASSERT(avatarStorage->hasAvatar(avatar1Hash));
			CPPUNIT_ASSERT_EQUAL(avatar1, avatarStorage->getAvatar(avatar1Hash));
		}

		void testGetAvatarHashUnknownAvatarUnknownVCard() {
			boost::shared_ptr<VCardAvatarManager> testling = createManager();

			std::string result = testling->getAvatarHash(user1);

			CPPUNIT_ASSERT_EQUAL(std::string(), result);
		}

		void testGetAvatarHashKnownAvatarUnknownVCard() {
			boost::shared_ptr<VCardAvatarManager> testling = createManager();
			vcardStorage->photoHashes[user1.toBare()] = avatar1Hash;
			
			std::string result = testling->getAvatarHash(user1);
			
			CPPUNIT_ASSERT_EQUAL(std::string(), result);
		} 


		void testVCardUpdateTriggersUpdate() {
			boost::shared_ptr<VCardAvatarManager> testling = createManager();
			vcardManager->requestVCard(user1);
			sendVCardResult();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
		}

	private:
		boost::shared_ptr<VCardAvatarManager> createManager() {
			boost::shared_ptr<VCardAvatarManager> result(new VCardAvatarManager(vcardManager, avatarStorage, mucRegistry));
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
		TestVCardStorage* vcardStorage;
		ByteArray avatar1;
		std::string avatar1Hash;
		std::vector<JID> changes;
		JID user1;
		JID user2;
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardAvatarManagerTest);
