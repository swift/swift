#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swift/Controllers/RosterController.h"
#include "Swift/Controllers/UnitTest/MockMainWindowFactory.h"
#include "Swiften/Roster/UnitTest/MockTreeWidgetFactory.h"
// #include "Swiften/Elements/Payload.h"
// #include "Swiften/Elements/RosterItemPayload.h"
// #include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swift/Controllers/EventController.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swift/Controllers/NickResolver.h"

using namespace Swift;

class RosterControllerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterControllerTest);
		CPPUNIT_TEST(testAdd);
		CPPUNIT_TEST_SUITE_END();

	public:
		RosterControllerTest() {};

		void setUp() {
			jid_ = JID("testjid@swift.im/swift");
			xmppRoster_ = boost::shared_ptr<XMPPRoster>(new XMPPRoster());
			avatarManager_ = NULL;//new AvatarManager();
			treeWidgetFactory_ = new MockTreeWidgetFactory();
			mainWindowFactory_ = new MockMainWindowFactory(treeWidgetFactory_);
			nickResolver_ = new NickResolver(xmppRoster_);
			channel_ = new DummyIQChannel();
			router_ = new IQRouter(channel_);
			stanzaChannel_ = new DummyStanzaChannel();
			presenceOracle_ = new PresenceOracle(stanzaChannel_);
			eventController_ = new EventController();
			rosterController_ = new RosterController(jid_, xmppRoster_, avatarManager_, mainWindowFactory_, treeWidgetFactory_, nickResolver_, presenceOracle_, eventController_);


		};

		void tearDown() {
			delete rosterController_;
			delete nickResolver_;
			delete treeWidgetFactory_;
			delete mainWindowFactory_;
			delete avatarManager_;
			delete channel_;
			delete router_;
			delete eventController_;
			delete presenceOracle_;
			delete stanzaChannel_;
		};

		void testAdd() {
			std::vector<String> groups;
			groups.push_back("testGroup1");
			groups.push_back("testGroup2");
			xmppRoster_->addContact(JID("test@testdomain.com/bob"), "name", groups);
			
			CPPUNIT_ASSERT_EQUAL(2, (int)treeWidgetFactory_->getGroups().size());
			//CPPUNIT_ASSERT_EQUAL(String("Bob"), xmppRoster_->getNameForJID(JID("foo@bar.com")));
		};

	private:
		JID jid_;
		boost::shared_ptr<XMPPRoster> xmppRoster_;
		AvatarManager* avatarManager_;
		MainWindowFactory* mainWindowFactory_;
		MockTreeWidgetFactory* treeWidgetFactory_;
		NickResolver* nickResolver_;
		RosterController* rosterController_;
		DummyIQChannel* channel_;
		DummyStanzaChannel* stanzaChannel_;	
		IQRouter* router_;
		PresenceOracle* presenceOracle_;
		EventController* eventController_;

};
