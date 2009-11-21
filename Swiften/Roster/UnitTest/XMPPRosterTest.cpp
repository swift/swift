#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <vector>

#include "Swiften/Roster/XMPPRoster.h"


using namespace Swift;

enum XMPPRosterEvents {None, Add, Remove, Update};

class XMPPRosterSignalHandler {
public:
	XMPPRosterSignalHandler(XMPPRoster* roster) {
		lastEvent_ = None;
		roster->onJIDAdded.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDAdded, this, _1));
		roster->onJIDRemoved.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDRemoved, this, _1));
		roster->onJIDUpdated.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDUpdated, this, _1, _2, _3));
	}

	XMPPRosterEvents getLastEvent() {
		return lastEvent_;
	}

	JID getLastJID() {
		return lastJID_;
	}

	String getLastOldName() {
		return lastOldName_;
	}

	std::vector<String> getLastOldGroups() {
		return lastOldGroups_;
	}

	void reset() {
		lastEvent_ = None;
	}

private:
	void handleJIDAdded(const JID& jid) {
		lastJID_ = jid;
		lastEvent_ = Add;
	}

	void handleJIDRemoved(const JID& jid) {
		lastJID_ = jid;
		lastEvent_ = Remove;
	}

	void handleJIDUpdated(const JID& jid, const String& oldName, const std::vector<String>& oldGroups) {
		CPPUNIT_ASSERT_EQUAL(None, lastEvent_);
		lastJID_ = jid;
		lastOldName_ = oldName;
		lastOldGroups_ = oldGroups;
		lastEvent_ = Update;
	}

	XMPPRosterEvents lastEvent_;
	JID lastJID_;
	String lastOldName_;
	std::vector<String> lastOldGroups_;

};

class XMPPRosterTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(XMPPRosterTest);
		CPPUNIT_TEST(testJIDAdded);
		CPPUNIT_TEST_SUITE_END();

	private:
		XMPPRoster* roster_;
		XMPPRosterSignalHandler* handler_;
		JID jid1_;
		JID jid2_; 
		JID jid3_;
		std::vector<String> groups1_;
		std::vector<String> groups2_;


	public:

		XMPPRosterTest() : jid1_(JID("a@b.c")), jid2_(JID("b@c.d")), jid3_(JID("c@d.e")) {}

		void setUp() {
			roster_ = new XMPPRoster();
			handler_ = new XMPPRosterSignalHandler(roster_);
			groups1_.push_back("bobs");
			groups1_.push_back("berts");
			groups2_.push_back("ernies");
		}

		void tearDown() {
			delete roster_;
		}

		void testJIDAdded() {
			roster_->addContact(jid1_, "NewName", groups1_);
			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(String("NewName"), roster_->getNameForJID(jid1_));
			//CPPUNIT_ASSERT_EQUAL(groups1_, roster_->getGroupsForJID(jid1_));
			handler_->reset();
			roster_->addContact(jid2_, "NameTwo", groups1_);
			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid2_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(String("NameTwo"), roster_->getNameForJID(jid2_));
			CPPUNIT_ASSERT_EQUAL(String("NewName"), roster_->getNameForJID(jid1_));
			//CPPUNIT_ASSERT_EQUAL(groups1_, roster_->getGroupsForJID(jid2_));
			//CPPUNIT_ASSERT_EQUAL(groups1_, roster_->getGroupsForJID(jid1_));
			handler_->reset();
			roster_->addContact(jid3_, "NewName", groups2_);
			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid3_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(String("NewName"), roster_->getNameForJID(jid3_));
			//CPPUNIT_ASSERT_EQUAL(groups2_, roster_->getGroupsForJID(jid3_));
		}

};
CPPUNIT_TEST_SUITE_REGISTRATION(XMPPRosterTest);

