/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Avatars/AvatarMemoryStorage.h>
#include <Swiften/Avatars/CombinedAvatarProvider.h>
#include <Swiften/Avatars/OfflineAvatarManager.h>
#include <Swiften/Avatars/VCardAvatarManager.h>
#include <Swiften/Avatars/VCardUpdateAvatarManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>

using namespace Swift;

class CombinedAvatarProviderTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(CombinedAvatarProviderTest);
		CPPUNIT_TEST(testGetAvatarWithNoAvatarProviderReturnsEmpty);
		CPPUNIT_TEST(testGetAvatarWithSingleAvatarProvider);
		CPPUNIT_TEST(testGetAvatarWithMultipleAvatarProviderReturnsFirstAvatar);
		CPPUNIT_TEST(testGetAvatarWithMultipleAvatarProviderAndFailingFirstProviderReturnsSecondAvatar);
		CPPUNIT_TEST(testProviderUpdateTriggersChange);
		CPPUNIT_TEST(testProviderUpdateWithoutChangeDoesNotTriggerChange);
		CPPUNIT_TEST(testProviderSecondUpdateTriggersChange);
		CPPUNIT_TEST(testProviderUpdateWithAvatarDisappearingTriggersChange);
		CPPUNIT_TEST(testProviderUpdateAfterAvatarDisappearedTriggersChange);
		CPPUNIT_TEST(testProviderUpdateAfterGetDoesNotTriggerChange);
		CPPUNIT_TEST(testProviderUpdateBareJIDAfterGetFullJID);
		CPPUNIT_TEST(testRemoveProviderDisconnectsUpdates);
		CPPUNIT_TEST(testAddRemoveFallthrough);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			avatarProvider1 = new DummyAvatarProvider();
			avatarProvider2 = new DummyAvatarProvider();
			user1 = JID("user1@bar.com/bla");
			user2 = JID("user2@foo.com/baz");
			avatarHash1 = "ABCDEFG";
			avatarHash2 = "XYZU";
			avatarHash3 = "IDGH";
		}

		void tearDown() {
			delete avatarProvider1;
			delete avatarProvider2;
		}

		void testGetAvatarWithNoAvatarProviderReturnsEmpty() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());

			boost::optional<std::string> hash = testling->getAvatarHash(user1);
			CPPUNIT_ASSERT(!hash);
		}

		void testGetAvatarWithSingleAvatarProvider() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			avatarProvider1->avatars[user1] = avatarHash1;
			testling->addProvider(avatarProvider1);

			boost::optional<std::string> hash = testling->getAvatarHash(user1);
			CPPUNIT_ASSERT(hash);
			CPPUNIT_ASSERT_EQUAL(avatarHash1, *hash);
		}

		void testGetAvatarWithMultipleAvatarProviderReturnsFirstAvatar() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider2->avatars[user1] = avatarHash2;
			testling->addProvider(avatarProvider1);
			testling->addProvider(avatarProvider2);

			boost::optional<std::string> hash = testling->getAvatarHash(user1);
			CPPUNIT_ASSERT(hash);
			CPPUNIT_ASSERT_EQUAL(avatarHash1, *hash);
		}

		void testGetAvatarWithMultipleAvatarProviderAndFailingFirstProviderReturnsSecondAvatar() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			avatarProvider2->avatars[user1] = avatarHash2;
			testling->addProvider(avatarProvider1);
			testling->addProvider(avatarProvider2);

			boost::optional<std::string> hash = testling->getAvatarHash(user1);
			CPPUNIT_ASSERT(hash);
			CPPUNIT_ASSERT_EQUAL(avatarHash2, *hash);
		}

		void testProviderUpdateTriggersChange() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			testling->addProvider(avatarProvider1);
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider1->onAvatarChanged(user1);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
		}

		void testProviderUpdateWithoutChangeDoesNotTriggerChange() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			testling->addProvider(avatarProvider1);
			testling->addProvider(avatarProvider2);
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider1->onAvatarChanged(user1);
			changes.clear();

			avatarProvider2->avatars[user1] = avatarHash2;
			avatarProvider2->onAvatarChanged(user1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
		}

		void testProviderSecondUpdateTriggersChange() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			testling->addProvider(avatarProvider1);
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider1->onAvatarChanged(user1);
			changes.clear();
			avatarProvider1->avatars[user1] = avatarHash2;
			avatarProvider1->onAvatarChanged(user1);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
		}


		void testProviderUpdateWithAvatarDisappearingTriggersChange() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			testling->addProvider(avatarProvider1);
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider1->onAvatarChanged(user1);
			changes.clear();
			avatarProvider1->avatars.clear();
			avatarProvider1->onAvatarChanged(user1);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
		}

		void testProviderUpdateAfterAvatarDisappearedTriggersChange() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			testling->addProvider(avatarProvider1);
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider1->onAvatarChanged(user1);
			avatarProvider1->avatars.clear();
			avatarProvider1->onAvatarChanged(user1);
			changes.clear();
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider1->onAvatarChanged(user1);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(user1, changes[0]);
		}


		void testProviderUpdateAfterGetDoesNotTriggerChange() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			testling->addProvider(avatarProvider1);
			avatarProvider1->avatars[user1] = avatarHash1;

			testling->getAvatarHash(user1);
			avatarProvider1->onAvatarChanged(user1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
		}

		void testRemoveProviderDisconnectsUpdates() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			testling->addProvider(avatarProvider1);
			testling->addProvider(avatarProvider2);
			testling->removeProvider(avatarProvider1);
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider2->avatars[user1] = avatarHash2;
			avatarProvider1->onAvatarChanged(user1);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
		}

		void testProviderUpdateBareJIDAfterGetFullJID() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			avatarProvider1->useBare = true;
			testling->addProvider(avatarProvider1);

			avatarProvider1->avatars[user1.toBare()] = avatarHash1;
			testling->getAvatarHash(user1);
			avatarProvider1->avatars[user1.toBare()] = avatarHash2;
			avatarProvider1->onAvatarChanged(user1.toBare());

			boost::optional<std::string> hash = testling->getAvatarHash(user1);
			CPPUNIT_ASSERT(hash);
			CPPUNIT_ASSERT_EQUAL(avatarHash2, *hash);
		}

		void testAddRemoveFallthrough() {
				JID ownJID = JID("user0@own.com/res");
				JID user1 = JID("user1@bar.com/bla");

				boost::shared_ptr<CryptoProvider> crypto = boost::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
				DummyStanzaChannel* stanzaChannel = new DummyStanzaChannel();
				stanzaChannel->setAvailable(true);
				IQRouter* iqRouter = new IQRouter(stanzaChannel);
				DummyMUCRegistry* mucRegistry = new DummyMUCRegistry();
				AvatarMemoryStorage* avatarStorage = new AvatarMemoryStorage();
				VCardMemoryStorage* vcardStorage = new VCardMemoryStorage(crypto.get());
				VCardManager* vcardManager = new VCardManager(ownJID, iqRouter, vcardStorage);

				boost::shared_ptr<VCardUpdateAvatarManager> updateManager(new VCardUpdateAvatarManager(vcardManager, stanzaChannel, avatarStorage, crypto.get(), mucRegistry));
				updateManager->onAvatarChanged.connect(boost::bind(&CombinedAvatarProviderTest::handleAvatarChanged, this, _1));

				boost::shared_ptr<VCardAvatarManager> manager(new VCardAvatarManager(vcardManager, avatarStorage, crypto.get(), mucRegistry));
				manager->onAvatarChanged.connect(boost::bind(&CombinedAvatarProviderTest::handleAvatarChanged, this, _1));

				boost::shared_ptr<OfflineAvatarManager> offlineManager(new OfflineAvatarManager(avatarStorage));
				offlineManager->onAvatarChanged.connect(boost::bind(&CombinedAvatarProviderTest::handleAvatarChanged, this, _1));

				boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
				avatarProvider1->useBare = true;
				testling->addProvider(updateManager.get());
				testling->addProvider(manager.get());
				testling->addProvider(offlineManager.get());

				/* place an avatar in the cache, check that it reads back OK */

				CPPUNIT_ASSERT_EQUAL(size_t(0), changes.size());

				ByteArray avatar1 = createByteArray("abcdefg");
				std::string avatar1Hash = Hexify::hexify(crypto->getSHA1Hash(avatar1));
				VCard::ref vcard1(new VCard());
				vcard1->setPhoto(avatar1);

				vcardStorage->setVCard(user1.toBare(), vcard1);
				boost::optional<std::string> testHash = testling->getAvatarHash(user1.toBare());
				CPPUNIT_ASSERT(testHash);
				CPPUNIT_ASSERT_EQUAL(avatar1Hash, *testHash);

				VCard::ref storedVCard = vcardStorage->getVCard(user1.toBare());
				CPPUNIT_ASSERT(!!storedVCard);
				testHash = Hexify::hexify(crypto->getSHA1Hash(storedVCard->getPhoto()));
				CPPUNIT_ASSERT_EQUAL(avatar1Hash, *testHash);

				/* change the avatar by sending an VCard IQ */

				vcardManager->requestVCard(user1.toBare());
				CPPUNIT_ASSERT_EQUAL(size_t(1), stanzaChannel->sentStanzas.size());
				IQ::ref request = boost::dynamic_pointer_cast<IQ>(stanzaChannel->sentStanzas.back());
				VCard::ref payload = request->getPayload<VCard>();
				CPPUNIT_ASSERT(!!payload);
				stanzaChannel->sentStanzas.pop_back();

				ByteArray avatar2 = createByteArray("1234567");
				std::string avatar2Hash = Hexify::hexify(crypto->getSHA1Hash(avatar2));
				VCard::ref vcard2(new VCard());
				vcard2->setPhoto(avatar2);

				IQ::ref reply = boost::make_shared<IQ>();
				reply->setTo(request->getFrom());
				reply->setFrom(request->getTo());
				reply->setID(request->getID());
				reply->addPayload(vcard2);
				reply->setType(IQ::Result);

				stanzaChannel->onIQReceived(reply);

				/* check that we changed the avatar successfully and that we were notified about the changes */

				testHash = testling->getAvatarHash(user1.toBare());
				CPPUNIT_ASSERT(testHash);
				CPPUNIT_ASSERT_EQUAL(avatar2Hash, *testHash);
				CPPUNIT_ASSERT_EQUAL(size_t(3), changes.size());
				CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[0]);
				CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[1]);
				CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[2]);
				changes.clear();
				storedVCard = vcardStorage->getVCard(user1.toBare());
				CPPUNIT_ASSERT(!!storedVCard);
				testHash = Hexify::hexify(crypto->getSHA1Hash(storedVCard->getPhoto()));
				CPPUNIT_ASSERT_EQUAL(avatar2Hash, *testHash);

				/* change the avatar to the empty avatar */

				vcardManager->requestVCard(user1.toBare());
				CPPUNIT_ASSERT_EQUAL(size_t(1), stanzaChannel->sentStanzas.size());
				request = boost::dynamic_pointer_cast<IQ>(stanzaChannel->sentStanzas.back());
				payload = request->getPayload<VCard>();
				CPPUNIT_ASSERT(!!payload);
				stanzaChannel->sentStanzas.pop_back();

				VCard::ref vcard3(new VCard());
				reply = boost::make_shared<IQ>();
				reply->setTo(request->getFrom());
				reply->setFrom(request->getTo());
				reply->setID(request->getID());
				reply->addPayload(vcard3);
				reply->setType(IQ::Result);
				stanzaChannel->onIQReceived(reply);

				/* check that we changed the avatar successfully */

				testHash = testling->getAvatarHash(user1.toBare());
				CPPUNIT_ASSERT(testHash);
				CPPUNIT_ASSERT_EQUAL(std::string(""), *testHash);
				CPPUNIT_ASSERT_EQUAL(size_t(3), changes.size());
				CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[0]);
				CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[1]);
				CPPUNIT_ASSERT_EQUAL(user1.toBare(), changes[2]);
				changes.clear();
				storedVCard = vcardStorage->getVCard(user1.toBare());
				CPPUNIT_ASSERT(!!storedVCard);
				CPPUNIT_ASSERT(!storedVCard->getPhoto().size());

				delete vcardManager;
				delete vcardStorage;
				delete mucRegistry;
				delete iqRouter;
				delete stanzaChannel;
		}

	private:
		boost::shared_ptr<CombinedAvatarProvider> createProvider() {
			boost::shared_ptr<CombinedAvatarProvider> result(new CombinedAvatarProvider());
			result->onAvatarChanged.connect(boost::bind(&CombinedAvatarProviderTest::handleAvatarChanged, this, _1));
			return result;
		}

		void handleAvatarChanged(const JID& jid) {
			changes.push_back(jid);
		}

	private:
		struct DummyAvatarProvider : public AvatarProvider {
			DummyAvatarProvider() : useBare(false) {
			}

			boost::optional<std::string> getAvatarHash(const JID& jid) const {
				JID actualJID = useBare ? jid.toBare() : jid;
				std::map<JID, std::string>::const_iterator i = avatars.find(actualJID);
				if (i != avatars.end()) {
					return i->second;
				}
				else {
					return boost::optional<std::string>();
				}
			}

			bool useBare;
			std::map<JID, std::string> avatars;
		};

		struct DummyMUCRegistry : public MUCRegistry {
			bool isMUC(const JID& jid) const { return std::find(mucs_.begin(), mucs_.end(), jid) != mucs_.end(); }
			std::vector<JID> mucs_;
		};

		DummyAvatarProvider* avatarProvider1;
		DummyAvatarProvider* avatarProvider2;
		JID user1;
		JID user2;
		std::string avatarHash1;
		std::string avatarHash2;
		std::string avatarHash3;
		std::vector<JID> changes;
};

CPPUNIT_TEST_SUITE_REGISTRATION(CombinedAvatarProviderTest);
