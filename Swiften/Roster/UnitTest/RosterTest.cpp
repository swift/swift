/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>

#include "Swiften/Roster/Roster.h"
#include "Swiften/Roster/GroupRosterItem.h"

using namespace Swift;

class RosterTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterTest);
		CPPUNIT_TEST(testGetGroup);
		CPPUNIT_TEST(testRemoveContact);
		CPPUNIT_TEST(testRemoveSecondContact);
		CPPUNIT_TEST(testRemoveSecondContactSameBare);
		CPPUNIT_TEST_SUITE_END();

	private:
		Roster *roster_;
		JID jid1_;
		JID jid2_; 
		JID jid3_;

	public:

		RosterTest() : jid1_(JID("a@b.c")), jid2_(JID("b@c.d")), jid3_(JID("c@d.e")) {}

		void setUp() {
			roster_ = new Roster();
		}

		void tearDown() {
			delete roster_;
		}

		void testGetGroup() {
			roster_->addContact(jid1_, "Bert", "group1");
			roster_->addContact(jid2_, "Ernie", "group2");
			roster_->addContact(jid3_, "Cookie", "group1");

			CPPUNIT_ASSERT_EQUAL(2, (int)roster_->getRoot()->getChildren().size());
			CPPUNIT_ASSERT_EQUAL(String("group1"), roster_->getRoot()->getChildren()[0]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(String("group2"), roster_->getRoot()->getChildren()[1]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(String("Bert"), ((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren()[0]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(String("Cookie"), ((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren()[1]->getDisplayName());
			CPPUNIT_ASSERT_EQUAL(String("Ernie"), ((GroupRosterItem*)roster_->getRoot()->getChildren()[1])->getChildren()[0]->getDisplayName());

		}

		void testRemoveContact() {
			roster_->addContact(jid1_, "Bert", "group1");
			CPPUNIT_ASSERT_EQUAL(String("Bert"), ((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren()[0]->getDisplayName());

			roster_->removeContact(jid1_);
			CPPUNIT_ASSERT_EQUAL(0, (int)((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren().size());
		}

		void testRemoveSecondContact() {
			roster_->addContact(jid1_, "Bert", "group1");
			roster_->addContact(jid2_, "Cookie", "group1");
			CPPUNIT_ASSERT_EQUAL(String("Cookie"), ((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren()[1]->getDisplayName());

			roster_->removeContact(jid2_);
			CPPUNIT_ASSERT_EQUAL(1, (int)((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren().size());
			CPPUNIT_ASSERT_EQUAL(String("Bert"), ((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren()[0]->getDisplayName());
		}

		void testRemoveSecondContactSameBare() {
			JID jid4a("a@b/c");
			JID jid4b("a@b/d");
			roster_->addContact(jid4a, "Bert", "group1");
			roster_->addContact(jid4b, "Cookie", "group1");
			CPPUNIT_ASSERT_EQUAL(String("Cookie"), ((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren()[1]->getDisplayName());

			roster_->removeContact(jid4b);
			CPPUNIT_ASSERT_EQUAL(1, (int)((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren().size());
			CPPUNIT_ASSERT_EQUAL(String("Bert"), ((GroupRosterItem*)roster_->getRoot()->getChildren()[0])->getChildren()[0]->getDisplayName());
		}

};
CPPUNIT_TEST_SUITE_REGISTRATION(RosterTest);

