/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

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
            roster_ = std::unique_ptr<XMPPRosterImpl>(new XMPPRosterImpl());
            handler_ = std::unique_ptr<XMPPRosterSignalHandler>(new XMPPRosterSignalHandler(roster_.get()));
            groups1_.push_back("bobs");
            groups1_.push_back("berts");
            groups2_.push_back("ernies");
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
        std::unique_ptr<XMPPRosterImpl> roster_;
        std::unique_ptr<XMPPRosterSignalHandler> handler_;
        JID jid1_;
        JID jid2_;
        JID jid3_;
        std::vector<std::string> groups1_;
        std::vector<std::string> groups2_;
};
CPPUNIT_TEST_SUITE_REGISTRATION(XMPPRosterImplTest);

