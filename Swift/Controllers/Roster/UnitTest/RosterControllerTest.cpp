
/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/foreach.h>
#include "Swift/Controllers/Roster/RosterController.h"
#include "Swift/Controllers/UnitTest/MockMainWindowFactory.h"
// #include "Swiften/Elements/Payload.h"
// #include "Swiften/Elements/RosterItemPayload.h"
// #include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Roster/XMPPRosterImpl.h"
#include "Swift/Controllers/Roster/Roster.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Settings/DummySettingsProvider.h"
#include "Swiften/Avatars/NullAvatarManager.h"
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Presence/SubscriptionManager.h"
#include "Swiften/Client/NickResolver.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RenameRosterItemUIEvent.h"
#include "Swiften/MUC/MUCRegistry.h"
#include <Swiften/Client/DummyNickManager.h>
#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/Disco/CapsProvider.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/FileTransfer/UnitTest/DummyFileTransferManager.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/EventLoop/DummyEventLoop.h>

using namespace Swift;

#define CHILDREN mainWindow_->roster->getRoot()->getChildren()

class DummyCapsProvider : public CapsProvider {
		DiscoInfo::ref getCaps(const std::string&) const {return DiscoInfo::ref(new DiscoInfo());}
};

class RosterControllerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(RosterControllerTest);
		CPPUNIT_TEST(testAdd);
		CPPUNIT_TEST(testAddSubscription);
		CPPUNIT_TEST(testReceiveRename);
		CPPUNIT_TEST(testReceiveRegroup);
		CPPUNIT_TEST(testSendRename);
		CPPUNIT_TEST(testPresence);
		CPPUNIT_TEST(testHighestPresence);
		CPPUNIT_TEST(testNotHighestPresence);
		CPPUNIT_TEST(testUnavailablePresence);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			jid_ = JID("testjid@swift.im/swift");
			xmppRoster_ = new XMPPRosterImpl();
			avatarManager_ = new NullAvatarManager();
			mainWindowFactory_ = new MockMainWindowFactory();
			mucRegistry_ = new MUCRegistry();
			nickResolver_ = new NickResolver(jid_.toBare(), xmppRoster_, NULL, mucRegistry_);
			channel_ = new DummyIQChannel();
			router_ = new IQRouter(channel_);
			stanzaChannel_ = new DummyStanzaChannel();
			presenceOracle_ = new PresenceOracle(stanzaChannel_);
			subscriptionManager_ = new SubscriptionManager(stanzaChannel_);
			eventController_ = new EventController();
			uiEventStream_ = new UIEventStream();
			settings_ = new DummySettingsProvider();
			nickManager_ = new DummyNickManager();
			capsProvider_ = new DummyCapsProvider();
			entityCapsManager_ = new EntityCapsManager(capsProvider_, stanzaChannel_);
			jingleSessionManager_ = new JingleSessionManager(router_);

			ftManager_ = new DummyFileTransferManager();
			ftOverview_ = new FileTransferOverview(ftManager_);
			rosterController_ = new RosterController(jid_, xmppRoster_, avatarManager_, mainWindowFactory_, nickManager_, nickResolver_, presenceOracle_, subscriptionManager_, eventController_, uiEventStream_, router_, settings_, entityCapsManager_, ftOverview_);
			mainWindow_ = mainWindowFactory_->last;
		};

		void tearDown() {
			delete rosterController_;
			delete ftManager_;
			delete jingleSessionManager_;
			
			delete nickManager_;
			delete nickResolver_;
			delete mucRegistry_;
			delete mainWindowFactory_;
			delete avatarManager_;
			delete router_;
			delete channel_;
			delete eventController_;
			delete subscriptionManager_;
			delete presenceOracle_;
			delete stanzaChannel_;
			delete uiEventStream_;
			delete settings_;
			delete xmppRoster_;
		};

	GroupRosterItem* groupChild(size_t i) {
		return dynamic_cast<GroupRosterItem*>(CHILDREN[i]);
	}

	JID withResource(const JID& jid, const std::string& resource) {
		return JID(jid.toBare().toString() + "/" + resource);
	}

	void testPresence() {
		std::vector<std::string> groups;
		groups.push_back("testGroup1");
		groups.push_back("testGroup2");
		JID from("test@testdomain.com");
		xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);
		Presence::ref presence(new Presence());
		presence->setFrom(withResource(from, "bob"));
		presence->setPriority(2);
		presence->setStatus("So totally here");
		stanzaChannel_->onPresenceReceived(presence);
		ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(CHILDREN[0])->getChildren()[0]);
		CPPUNIT_ASSERT(item);
		CPPUNIT_ASSERT_EQUAL(presence->getStatus(), item->getStatusText());
		ContactRosterItem* item2 = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(CHILDREN[1])->getChildren()[0]);
		CPPUNIT_ASSERT(item2);
		CPPUNIT_ASSERT_EQUAL(presence->getStatus(), item2->getStatusText());

	};

	void testHighestPresence() {
		std::vector<std::string> groups;
		groups.push_back("testGroup1");
		JID from("test@testdomain.com");
		xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);
		Presence::ref lowPresence(new Presence());
		lowPresence->setFrom(withResource(from, "bob"));
		lowPresence->setPriority(2);
		lowPresence->setStatus("Not here");
		Presence::ref highPresence(new Presence());
		highPresence->setFrom(withResource(from, "bert"));
		highPresence->setPriority(10);
		highPresence->setStatus("So totally here");
		stanzaChannel_->onPresenceReceived(lowPresence);
		stanzaChannel_->onPresenceReceived(highPresence);
		ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(CHILDREN[0])->getChildren()[0]);
		CPPUNIT_ASSERT(item);
		CPPUNIT_ASSERT_EQUAL(highPresence->getStatus(), item->getStatusText());
	};

	void testNotHighestPresence() {
		std::vector<std::string> groups;
		groups.push_back("testGroup1");
		JID from("test@testdomain.com");
		xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);
		Presence::ref lowPresence(new Presence());
		lowPresence->setFrom(withResource(from, "bob"));
		lowPresence->setPriority(2);
		lowPresence->setStatus("Not here");
		Presence::ref highPresence(new Presence());
		highPresence->setFrom(withResource(from, "bert"));
		highPresence->setPriority(10);
		highPresence->setStatus("So totally here");
		stanzaChannel_->onPresenceReceived(highPresence);
		stanzaChannel_->onPresenceReceived(lowPresence);
		ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(CHILDREN[0])->getChildren()[0]);
		CPPUNIT_ASSERT(item);
		CPPUNIT_ASSERT_EQUAL(highPresence->getStatus(), item->getStatusText());
	};

	void testUnavailablePresence() {
		std::vector<std::string> groups;
		groups.push_back("testGroup1");
		JID from("test@testdomain.com");
		xmppRoster_->addContact(from, "name", groups, RosterItemPayload::Both);
		Presence::ref lowPresence(new Presence());
		lowPresence->setFrom(withResource(from, "bob"));
		lowPresence->setPriority(2);
		lowPresence->setStatus("Not here");
		Presence::ref highPresence(new Presence());
		highPresence->setFrom(withResource(from, "bert"));
		highPresence->setPriority(10);
		highPresence->setStatus("So totally here");
		Presence::ref highPresenceOffline(new Presence());
		highPresenceOffline->setFrom(withResource(from, "bert"));
		highPresenceOffline->setType(Presence::Unavailable);
		Presence::ref lowPresenceOffline(new Presence());
		lowPresenceOffline->setFrom(withResource(from, "bob"));
		lowPresenceOffline->setStatus("Signing out");
		lowPresenceOffline->setType(Presence::Unavailable);
		stanzaChannel_->onPresenceReceived(lowPresence);
		stanzaChannel_->onPresenceReceived(highPresence);
		stanzaChannel_->onPresenceReceived(highPresenceOffline);
		ContactRosterItem* item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(CHILDREN[0])->getChildren()[0]);
		CPPUNIT_ASSERT(item);
		/* A verification that if the test fails, it's the RosterController, not the PresenceOracle. */
		Presence::ref high = presenceOracle_->getHighestPriorityPresence(from);
		CPPUNIT_ASSERT_EQUAL(Presence::Available, high->getType());
		CPPUNIT_ASSERT_EQUAL(lowPresence->getStatus(), high->getStatus());
		CPPUNIT_ASSERT_EQUAL(StatusShow::Online, item->getStatusShow());
		CPPUNIT_ASSERT_EQUAL(lowPresence->getStatus(), item->getStatusText());
		stanzaChannel_->onPresenceReceived(lowPresenceOffline);
		item = dynamic_cast<ContactRosterItem*>(dynamic_cast<GroupRosterItem*>(CHILDREN[0])->getChildren()[0]);
		CPPUNIT_ASSERT(item);
		/* A verification that if the test fails, it's the RosterController, not the PresenceOracle. */
		high = presenceOracle_->getHighestPriorityPresence(from);
		CPPUNIT_ASSERT_EQUAL(Presence::Unavailable, high->getType());
		CPPUNIT_ASSERT_EQUAL(lowPresenceOffline->getStatus(), high->getStatus());
		CPPUNIT_ASSERT_EQUAL(StatusShow::None, item->getStatusShow());
		CPPUNIT_ASSERT_EQUAL(lowPresenceOffline->getStatus(), item->getStatusText());
	};

		void testAdd() {
			std::vector<std::string> groups;
			groups.push_back("testGroup1");
			groups.push_back("testGroup2");
			xmppRoster_->addContact(JID("test@testdomain.com/bob"), "name", groups, RosterItemPayload::Both);
			
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(CHILDREN.size()));
			//CPPUNIT_ASSERT_EQUAL(std::string("Bob"), xmppRoster_->getNameForJID(JID("foo@bar.com")));
		};

 		void testAddSubscription() {
			std::vector<std::string> groups;
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
			std::vector<std::string> groups;
			JID jid("test@testdomain.com");
			xmppRoster_->addContact(jid, "name", groups, RosterItemPayload::Both);
			
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("name"), groupChild(0)->getChildren()[0]->getDisplayName());
			xmppRoster_->addContact(jid, "NewName", groups, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("NewName"), groupChild(0)->getChildren()[0]->getDisplayName());
		};

	void testReceiveRegroup() {
		std::vector<std::string> oldGroups;
		std::vector<std::string> newGroups;
		newGroups.push_back("A Group");
		std::vector<std::string> newestGroups;
		newestGroups.push_back("Best Group");
		JID jid("test@testdomain.com");
		xmppRoster_->addContact(jid, "", oldGroups, RosterItemPayload::Both);

		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
		CPPUNIT_ASSERT_EQUAL(jid.toString(), groupChild(0)->getChildren()[0]->getDisplayName());

		xmppRoster_->addContact(jid, "new name", newGroups, RosterItemPayload::Both);
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
		CPPUNIT_ASSERT_EQUAL(std::string("new name"), groupChild(0)->getChildren()[0]->getDisplayName());
		CPPUNIT_ASSERT_EQUAL(std::string("A Group"), groupChild(0)->getDisplayName());

		xmppRoster_->addContact(jid, "new name", newestGroups, RosterItemPayload::Both);
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(CHILDREN.size()));
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(groupChild(0)->getChildren().size()));
		CPPUNIT_ASSERT_EQUAL(std::string("new name"), groupChild(0)->getChildren()[0]->getDisplayName());
		CPPUNIT_ASSERT_EQUAL(std::string("Best Group"), groupChild(0)->getDisplayName());
	};

		void testSendRename() {
			JID jid("testling@wonderland.lit");
			std::vector<std::string> groups;
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
			CPPUNIT_ASSERT_EQUAL(std::string("Robert"), item.getName());

			CPPUNIT_ASSERT_EQUAL(groups.size(), item.getGroups().size());
			assertVectorsEqual(groups, item.getGroups(), __LINE__);
		}

		void assertVectorsEqual(const std::vector<std::string>& v1, const std::vector<std::string>& v2, int line) {
			foreach (const std::string& entry, v1) {
				if (std::find(v2.begin(), v2.end(), entry) == v2.end()) {
					std::stringstream stream;
					stream <<	"Couldn't find " << entry << " in v2 (line " << line << ")";
					CPPUNIT_FAIL(stream.str());
				}
			}
		}

	private:
		JID jid_;
		XMPPRosterImpl* xmppRoster_;
		MUCRegistry* mucRegistry_;
		AvatarManager* avatarManager_;
		MockMainWindowFactory* mainWindowFactory_;
		NickManager* nickManager_;
		NickResolver* nickResolver_;
		RosterController* rosterController_;
		DummyIQChannel* channel_;
		DummyStanzaChannel* stanzaChannel_;	
		IQRouter* router_;
		PresenceOracle* presenceOracle_;
		SubscriptionManager* subscriptionManager_;
		EventController* eventController_;
		UIEventStream* uiEventStream_;
		MockMainWindow* mainWindow_;
		DummySettingsProvider* settings_;
		DummyCapsProvider* capsProvider_;
		EntityCapsManager* entityCapsManager_;
		JingleSessionManager* jingleSessionManager_;
		FileTransferManager* ftManager_;
		FileTransferOverview* ftOverview_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterControllerTest);
