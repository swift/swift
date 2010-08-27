
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
#include "Swift/Controllers/EventController.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"

using namespace Swift;

#define CHILDREN mainWindow_->roster->getRoot()->getChildren()
class RosterControllerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterControllerTest);
		CPPUNIT_TEST(testAdd);
		CPPUNIT_TEST(testAddSubscription);
		CPPUNIT_TEST(testRename);
		CPPUNIT_TEST_SUITE_END();

	public:
		RosterControllerTest() {};

		void setUp() {
			jid_ = JID("testjid@swift.im/swift");
			xmppRoster_ = boost::shared_ptr<XMPPRoster>(new XMPPRoster());
			avatarManager_ = NULL;//new AvatarManager();
			mainWindowFactory_ = new MockMainWindowFactory();
			nickResolver_ = new NickResolver(jid_.toBare(), xmppRoster_, NULL);
			channel_ = new DummyIQChannel();
			router_ = new IQRouter(channel_);
			stanzaChannel_ = new DummyStanzaChannel();
			presenceOracle_ = new PresenceOracle(stanzaChannel_);
			eventController_ = new EventController();
			uiEventStream_ = new UIEventStream();
			rosterController_ = new RosterController(jid_, xmppRoster_, avatarManager_, mainWindowFactory_, nickResolver_, presenceOracle_, eventController_, uiEventStream_, router_);
			mainWindow_ = mainWindowFactory_->last;
		};

		void tearDown() {
			delete rosterController_;
			delete nickResolver_;
			delete mainWindowFactory_;
			delete avatarManager_;
			delete channel_;
			delete router_;
			delete eventController_;
			delete presenceOracle_;
			delete stanzaChannel_;
			delete uiEventStream_;
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

		void testRename() {
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

	private:
		JID jid_;
		boost::shared_ptr<XMPPRoster> xmppRoster_;
		AvatarManager* avatarManager_;
		MockMainWindowFactory* mainWindowFactory_;
		NickResolver* nickResolver_;
		RosterController* rosterController_;
		DummyIQChannel* channel_;
		DummyStanzaChannel* stanzaChannel_;	
		IQRouter* router_;
		PresenceOracle* presenceOracle_;
		EventController* eventController_;
		UIEventStream* uiEventStream_;
		MockMainWindow* mainWindow_;
};
#undef children

CPPUNIT_TEST_SUITE_REGISTRATION(RosterControllerTest);
