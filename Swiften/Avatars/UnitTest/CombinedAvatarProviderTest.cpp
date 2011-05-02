/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include <Swiften/JID/JID.h>
#include <string>
#include <Swiften/Avatars/CombinedAvatarProvider.h>

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

			CPPUNIT_ASSERT(testling->getAvatarHash(user1).empty());
		}

		void testGetAvatarWithSingleAvatarProvider() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			avatarProvider1->avatars[user1] = avatarHash1;
			testling->addProvider(avatarProvider1);

			CPPUNIT_ASSERT_EQUAL(avatarHash1, testling->getAvatarHash(user1));
		}

		void testGetAvatarWithMultipleAvatarProviderReturnsFirstAvatar() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			avatarProvider1->avatars[user1] = avatarHash1;
			avatarProvider2->avatars[user1] = avatarHash2;
			testling->addProvider(avatarProvider1);
			testling->addProvider(avatarProvider2);

			CPPUNIT_ASSERT_EQUAL(avatarHash1, testling->getAvatarHash(user1));
		}

		void testGetAvatarWithMultipleAvatarProviderAndFailingFirstProviderReturnsSecondAvatar() {
			boost::shared_ptr<CombinedAvatarProvider> testling(createProvider());
			avatarProvider2->avatars[user1] = avatarHash2;
			testling->addProvider(avatarProvider1);
			testling->addProvider(avatarProvider2);

			CPPUNIT_ASSERT_EQUAL(avatarHash2, testling->getAvatarHash(user1));
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

			CPPUNIT_ASSERT_EQUAL(avatarHash2, testling->getAvatarHash(user1));
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

			std::string getAvatarHash(const JID& jid) const {
				JID actualJID = useBare ? jid.toBare() : jid;
				std::map<JID, std::string>::const_iterator i = avatars.find(actualJID);
				if (i != avatars.end()) {
					return i->second;
				}
				else {
					return std::string();
				}
			}

			bool useBare;
			std::map<JID, std::string> avatars;
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
