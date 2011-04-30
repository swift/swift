/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <Swiften/Roster/UnitTest/XMPPRosterSignalHandler.h>
#include <Swiften/Roster/XMPPRosterImpl.h>


using namespace Swift;



class XMPPRosterImplTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(XMPPRosterImplTest);
		CPPUNIT_TEST(testJIDAdded);
		CPPUNIT_TEST(testJIDRemoved);
		CPPUNIT_TEST(testJIDUpdated);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			jid1_ = JID("a@b.c");
			jid2_ = JID("b@c.d");
			jid3_ = JID("c@d.e");
			roster_ = new XMPPRosterImpl();
			handler_ = new XMPPRosterSignalHandler(roster_);
			groups1_.push_back("bobs");
			groups1_.push_back("berts");
			groups2_.push_back("ernies");
		}

		void tearDown() {
			delete handler_;
			delete roster_;
		}

		void testJIDAdded() {
			roster_->addContact(jid1_, "NewName", groups1_, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(std::string("NewName"), roster_->getNameForJID(jid1_));
			CPPUNIT_ASSERT(groups1_ == roster_->getGroupsForJID(jid1_));
			handler_->reset();
			roster_->addContact(jid2_, "NameTwo", groups1_, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid2_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(std::string("NameTwo"), roster_->getNameForJID(jid2_));
			CPPUNIT_ASSERT_EQUAL(std::string("NewName"), roster_->getNameForJID(jid1_));
			CPPUNIT_ASSERT(groups1_ == roster_->getGroupsForJID(jid2_));
			CPPUNIT_ASSERT(groups1_ == roster_->getGroupsForJID(jid1_));
			handler_->reset();
			roster_->addContact(jid3_, "NewName", groups2_, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid3_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(std::string("NewName"), roster_->getNameForJID(jid3_));
			CPPUNIT_ASSERT(groups2_ == roster_->getGroupsForJID(jid3_));
		}

		void testJIDRemoved() {
			roster_->addContact(jid1_, "NewName", groups1_, RosterItemPayload::Both);
			handler_->reset();
			roster_->removeContact(jid1_);
			CPPUNIT_ASSERT_EQUAL(Remove, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			handler_->reset();
			roster_->addContact(jid1_, "NewName2", groups1_, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(std::string("NewName2"), roster_->getNameForJID(jid1_));
			roster_->addContact(jid2_, "NewName3", groups1_, RosterItemPayload::Both);
			handler_->reset();
			roster_->removeContact(jid2_);
			CPPUNIT_ASSERT_EQUAL(Remove, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid2_, handler_->getLastJID());
			handler_->reset();
			roster_->removeContact(jid1_);
			CPPUNIT_ASSERT_EQUAL(Remove, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
		}

		void testJIDUpdated() {
			roster_->addContact(jid1_, "NewName", groups1_, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(std::string("NewName"), roster_->getNameForJID(jid1_));
			CPPUNIT_ASSERT(groups1_ == roster_->getGroupsForJID(jid1_));
			handler_->reset();
			roster_->addContact(jid1_, "NameTwo", groups2_, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(Update, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(std::string("NameTwo"), roster_->getNameForJID(jid1_));
			CPPUNIT_ASSERT(groups2_ == roster_->getGroupsForJID(jid1_));
		}

	private:
		XMPPRosterImpl* roster_;
		XMPPRosterSignalHandler* handler_;
		JID jid1_;
		JID jid2_;
		JID jid3_;
		std::vector<std::string> groups1_;
		std::vector<std::string> groups2_;
};
CPPUNIT_TEST_SUITE_REGISTRATION(XMPPRosterImplTest);

