/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/Roster/Roster.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include "Swift/Controllers/Roster/SetPresence.h"

using namespace Swift;

class RosterTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(RosterTest);
		CPPUNIT_TEST(testGetGroup);
		CPPUNIT_TEST(testRemoveContact);
		CPPUNIT_TEST(testRemoveSecondContact);
		CPPUNIT_TEST(testRemoveSecondContactSameBare);
		CPPUNIT_TEST(testApplyPresenceLikeMUC);
		CPPUNIT_TEST(testReSortLikeMUC);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			jid1_ = JID("a@b.c");
			jid2_ = JID("b@c.d");
			jid3_ = JID("c@d.e");
			roster_ = new Roster();
		}

		void tearDown() {
			delete roster_;
		}

		void testGetGroup() {
			roster_->addContact(jid1_, JID(), "Bert", "group1", "");
			roster_->addContact(jid2_, JID(), "Ernie", "group2", "");
			roster_->addContact(jid3_, JID(), "Cookie", "group1", "");

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(roster_->getRoot()->getChildren().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("group1"), roster_->getRoot()->getChildren()[0]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(std::string("group2"), roster_->getRoot()->getChildren()[1]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(std::string("Bert"), static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren()[0]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(std::string("Cookie"), static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren()[1]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(std::string("Ernie"), static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[1])->getChildren()[0]->getDisplayName());

		}

		void testRemoveContact() {
			roster_->addContact(jid1_, jid1_, "Bert", "group1", "");
			CPPUNIT_ASSERT_EQUAL(std::string("Bert"), static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren()[0]->getDisplayName());

			roster_->removeContact(jid1_);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren().size()));
		}

		void testRemoveSecondContact() {
			roster_->addContact(jid1_, jid1_, "Bert", "group1", "");
			roster_->addContact(jid2_, jid2_, "Cookie", "group1", "");
			CPPUNIT_ASSERT_EQUAL(std::string("Cookie"), static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren()[1]->getDisplayName());

			roster_->removeContact(jid2_);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Bert"), static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren()[0]->getDisplayName());
		}

		void testRemoveSecondContactSameBare() {
			JID jid4a("a@b/c");
			JID jid4b("a@b/d");
			roster_->addContact(jid4a, JID(), "Bert", "group1", "");
			roster_->addContact(jid4b, JID(), "Cookie", "group1", "");
			CPPUNIT_ASSERT_EQUAL(std::string("Cookie"), static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren()[1]->getDisplayName());

			roster_->removeContact(jid4b);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Bert"), static_cast<GroupRosterItem*>(roster_->getRoot()->getChildren()[0])->getChildren()[0]->getDisplayName());
		}

		void testApplyPresenceLikeMUC() {
			JID jid4a("a@b/c");
			JID jid4b("a@b/d");
			JID jid4c("a@b/e");
			roster_->addContact(jid4a, JID(), "Bird", "group1", "");
			roster_->addContact(jid4b, JID(), "Cookie", "group1", "");
			roster_->removeContact(jid4b);
			roster_->addContact(jid4c, JID(), "Bert", "group1", "");
			roster_->addContact(jid4b, JID(), "Ernie", "group1", "");
			boost::shared_ptr<Presence> presence(new Presence());
			presence->setShow(StatusShow::DND);
			presence->setFrom(jid4a);
			roster_->applyOnItems(SetPresence(presence, JID::WithResource));
			presence->setFrom(jid4b);
			roster_->applyOnItems(SetPresence(presence, JID::WithResource));
			presence->setFrom(jid4c);
			roster_->applyOnItems(SetPresence(presence, JID::WithResource));
			
			presence = boost::make_shared<Presence>();
			presence->setFrom(jid4b);
			presence->setShow(StatusShow::Online);
			roster_->applyOnItems(SetPresence(presence, JID::WithResource));
			std::vector<RosterItem*> children = static_cast<GroupRosterItem*>(roster_->getRoot()->getDisplayedChildren()[0])->getDisplayedChildren();
			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(children.size()));
			
			/* Check order */
			CPPUNIT_ASSERT_EQUAL(std::string("Ernie"), children[0]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(std::string("Bert"), children[1]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(std::string("Bird"), children[2]->getDisplayName());

			presence = boost::make_shared<Presence>();
			presence->setFrom(jid4c);
			presence->setType(Presence::Unavailable);
			roster_->removeContact(jid4c);
			roster_->applyOnItems(SetPresence(presence, JID::WithResource));

		}

		void testReSortLikeMUC() {
			JID jid4a("a@b/c");
			JID jid4b("a@b/d");
			JID jid4c("a@b/e");
			roster_->addContact(jid4a, JID(), "Bird", "group1", "");
			roster_->addContact(jid4b, JID(), "Cookie", "group2", "");
			roster_->addContact(jid4b, JID(), "Ernie", "group1", "");
			roster_->getGroup("group1")->setManualSort("2");
			roster_->getGroup("group2")->setManualSort("1");
			GroupRosterItem* root = roster_->getRoot();
			const std::vector<RosterItem*> kids = root->getDisplayedChildren();
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), kids.size());
			CPPUNIT_ASSERT_EQUAL(std::string("group2"), kids[0]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(std::string("group1"), kids[1]->getDisplayName());
		}

	private:
		Roster *roster_;
		JID jid1_;
		JID jid2_; 
		JID jid3_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(RosterTest);

