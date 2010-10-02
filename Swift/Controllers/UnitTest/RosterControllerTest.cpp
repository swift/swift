
/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swift/Controllers/RosterController.h"
#include "Swift/Controllers/UnitTest/MockMainWindowFactory.h"
// #include "Swiften/Elements/Payload.h"
// #include "Swiften/Elements/RosterItemPayload.h"
// #include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swiften/Roster/Roster.h"
#include "Swiften/Roster/GroupRosterItem.h"
#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Settings/DummySettingsProvider.h"
#include "Swiften/Avatars/NullAvatarManager.h"
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RenameRosterItemUIEvent.h"
#include "Swift/Controllers/UIEvents/RegroupRosterItemUIEvent.h"
#include "Swiften/MUC/MUCRegistry.h"

using namespace Swift;

#define CHILDREN mainWindow_->roster->getRoot()->getChildren()

class RosterControllerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterControllerTest);
		CPPUNIT_TEST(testAdd);
		CPPUNIT_TEST(testAddSubscription);
		CPPUNIT_TEST(testReceiveRename);
		CPPUNIT_TEST(testSendRename);
		CPPUNIT_TEST(testSendRegroup);
		CPPUNIT_TEST_SUITE_END();

	public:
		RosterControllerTest() {};

		void setUp() {
			jid_ = JID("testjid@swift.im/swift");
			xmppRoster_ = new XMPPRoster();
			avatarManager_ = new NullAvatarManager();
			mainWindowFactory_ = new MockMainWindowFactory();
			mucRegistry_ = new MUCRegistry();
			nickResolver_ = new NickResolver(jid_.toBare(), xmppRoster_, NULL, mucRegistry_);
			channel_ = new DummyIQChannel();
			router_ = new IQRouter(channel_);
			stanzaChannel_ = new DummyStanzaChannel();
			presenceOracle_ = new PresenceOracle(stanzaChannel_);
			presenceSender_ = new PresenceSender(stanzaChannel_);
			eventController_ = new EventController();
			uiEventStream_ = new UIEventStream();
			settings_ = new DummySettingsProvider();
			rosterController_ = new RosterController(jid_, xmppRoster_, avatarManager_, mainWindowFactory_, nickResolver_, presenceOracle_, presenceSender_, eventController_, uiEventStream_, router_, settings_);
			mainWindow_ = mainWindowFactory_->last;
		};

		void tearDown() {
			delete rosterController_;
			delete nickResolver_;
			delete mucRegistry_;
			delete mainWindowFactory_;
			delete avatarManager_;
			delete channel_;
			delete router_;
			delete eventController_;
			delete presenceSender_;
			delete presenceOracle_;
			delete stanzaChannel_;
			delete uiEventStream_;
			delete settings_;
		};

	GroupRosterItem* groupChild(size_t i) {
		return dynamic_cast<GroupRosterItem*>(CHILDREN[i]);
	}

		void testAdd() {
			std::vector<String> groups;
			groups.push_back("testGroup1");
			groups.push_back("testGroup2");
			xmppRoster_->addContact(JID("test@testdomain.com/bob"), "name", groups, RosterItemPayload::Both);
			
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(CHILDREN.size()));
			//CPPUNIT_ASSERT_EQUAL(String("Bob"), xmppRoster_->getNameForJID(JID("foo@bar.com")));
		};

 		void testAddSubscription() {
			std::vector<String> groups;
			JID jid("test@testdomain.com");
			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::None);
			
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::To);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));

			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));

		};

		void testReceiveRename() {
			std::vector<String> groups;
			JID jid("test@testdomain.com");
			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::Both);
			
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
			CPPUNIT_ASSERT_EQUAL(String("name"), groupChild(0)->getChildren()[0]->getDisplayName());
			xmppRoster_->addContact(jid, "NewName", groups, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
			CPPUNIT_ASSERT_EQUAL(String("NewName"), groupChild(0)->getChildren()[0]->getDisplayName());
		};

		void testSendRename() {
			JID jid("testling@wonderland.lit");
			std::vector<String> groups;
			groups.push_back("Friends");
			groups.push_back("Enemies");
			xmppRoster_->addContact(jid, "Bob", groups, RosterItemPayload::From);
			CPPUNIT_ASSERT_EQUAL(groups.size(), xmppRoster_->getGroupsForJID(jid).size());
			uiEventStream_->send(boost::shared_ptr<UIEvent>(new RenameRosterItemUIEvent(jid, "Robert")));
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), channel_->iqs_.size());
			CPPUNIT_ASSERT_EQUAL(IQ::Set, channel_->iqs_[0]->getType());
			boost::shared_ptr<RosterPayload> payload = channel_->iqs_[0]->getPayload<RosterPayload>();
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), payload->getItems().size());
			RosterItemPayload item = payload->getItems()[0];
			CPPUNIT_ASSERT_EQUAL(jid, item.getJID());
			CPPUNIT_ASSERT_EQUAL(String("Robert"), item.getName());

			CPPUNIT_ASSERT_EQUAL(groups.size(), item.getGroups().size());
			assertVectorsEqual(groups, item.getGroups(), __LINE__);
		}

		void testSendRegroup() {
			JID jid("testling@wonderland.lit");
			String friends("Friends");
			String enemies("Ememies");
			String people("People");
			String contacts("Contacts");
			std::vector<String> oldGroups;
			oldGroups.push_back(friends);
			oldGroups.push_back(enemies);
			std::vector<String> newGroups;
			newGroups.push_back(friends);
			newGroups.push_back(people);
			newGroups.push_back(contacts);
			std::vector<String> addedGroups;
			addedGroups.push_back(people);
			addedGroups.push_back(contacts);
			std::vector<String> removedGroups;
			removedGroups.push_back(enemies);


			xmppRoster_->addContact(jid, "Bob", oldGroups, RosterItemPayload::From);
			CPPUNIT_ASSERT_EQUAL(oldGroups.size(), xmppRoster_->getGroupsForJID(jid).size());
			uiEventStream_->send(boost::shared_ptr<UIEvent>(new RegroupRosterItemUIEvent(jid, addedGroups, removedGroups)));
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), channel_->iqs_.size());
			CPPUNIT_ASSERT_EQUAL(IQ::Set, channel_->iqs_[0]->getType());
			boost::shared_ptr<RosterPayload> payload = channel_->iqs_[0]->getPayload<RosterPayload>();
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), payload->getItems().size());
			RosterItemPayload item = payload->getItems()[0];
			CPPUNIT_ASSERT_EQUAL(jid, item.getJID());
			CPPUNIT_ASSERT_EQUAL(String("Bob"), item.getName());

			CPPUNIT_ASSERT_EQUAL(newGroups.size(), item.getGroups().size());
			assertVectorsEqual(newGroups, item.getGroups(), __LINE__);
		}

		void assertVectorsEqual(const std::vector<String>& v1, const std::vector<String>& v2, int line) {
			foreach (const String& entry, v1) {
				if (std::find(v2.begin(), v2.end(), entry) == v2.end()) {
					std::stringstream stream;
					stream <<	"Couldn't find " << entry.getUTF8String() << " in v2 (line " << line << ")";
					CPPUNIT_FAIL(stream.str());
				}
			}
		}

	private:
		JID jid_;
		XMPPRoster* xmppRoster_;
		MUCRegistry* mucRegistry_;
		AvatarManager* avatarManager_;
		MockMainWindowFactory* mainWindowFactory_;
		NickResolver* nickResolver_;
		RosterController* rosterController_;
		DummyIQChannel* channel_;
		DummyStanzaChannel* stanzaChannel_;	
		IQRouter* router_;
		PresenceOracle* presenceOracle_;
		PresenceSender* presenceSender_;
		EventController* eventController_;
		UIEventStream* uiEventStream_;
		MockMainWindow* mainWindow_;
		DummySettingsProvider* settings_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterControllerTest);
