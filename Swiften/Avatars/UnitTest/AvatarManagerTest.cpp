#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Avatars/AvatarStorage.h"
#include "Swiften/MUC/MUCRegistry.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Client/DummyStanzaChannel.h"

using namespace Swift;

class AvatarManagerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(AvatarManagerTest);
		CPPUNIT_TEST(testUpdate_UpdateNewHash);
		CPPUNIT_TEST(testUpdate_UpdateNewHashAlreadyHaveAvatar);
		CPPUNIT_TEST(testUpdate_UpdateNewHashFromMUC);
		CPPUNIT_TEST(testUpdate_UpdateSameHash);
		CPPUNIT_TEST(testUpdate_UpdateNewHashSameThanOtherUser);
		CPPUNIT_TEST(testReceiveVCard);
		CPPUNIT_TEST(testGetAvatarPath);
		CPPUNIT_TEST(testGetAvatarPathFromMUC);
		CPPUNIT_TEST_SUITE_END();

	public:
		AvatarManagerTest() {}

		void setUp() {
			stanzaChannel_ = new DummyStanzaChannel();
			iqRouter_ = new IQRouter(stanzaChannel_);
			mucRegistry_ = new DummyMUCRegistry();
			avatarStorage_ = new DummyAvatarStorage();
		}

		void tearDown() {
			delete avatarStorage_;
			delete mucRegistry_;
			delete iqRouter_;
			delete stanzaChannel_;
		}

		void testUpdateNewHash() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

		void testUpdate_UpdateNewHash() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

		void testUpdate_UpdateNewHashAlreadyHaveAvatar() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

		void testUpdate_UpdateNewHashFromMUC() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

		void testUpdate_UpdateSameHash() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

		void testUpdate_UpdateNewHashSameThanOtherUser() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

		void testReceiveVCard() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

		void testGetAvatarPath() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

		void testGetAvatarPathFromMUC() {
			std::auto_ptr<AvatarManager> testling = createManager();
		}

	private:
		std::auto_ptr<AvatarManager> createManager() {
			return std::auto_ptr<AvatarManager>(new AvatarManager(stanzaChannel_, iqRouter_, avatarStorage_, mucRegistry_));
		}

	private:
		struct DummyMUCRegistry : public MUCRegistry {
			bool isMUC(const JID& jid) const { return std::find(mucs_.begin(), mucs_.end(), jid) != mucs_.end(); }
			std::vector<JID> mucs_;
		};
		struct DummyAvatarStorage : public AvatarStorage {
			virtual bool hasAvatar(const String& hash) const { return avatars.find(hash) != avatars.end(); }
			virtual void addAvatar(const String& hash, const ByteArray& avatar) { avatars[hash] = avatar; }
			virtual boost::filesystem::path getAvatarPath(const String& hash) const {
				return boost::filesystem::path("/avatars") / hash.getUTF8String();
			}
			std::map<String, ByteArray> avatars;
		};
		DummyStanzaChannel* stanzaChannel_;
		IQRouter* iqRouter_;
		DummyMUCRegistry* mucRegistry_;
		DummyAvatarStorage* avatarStorage_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(AvatarManagerTest);
