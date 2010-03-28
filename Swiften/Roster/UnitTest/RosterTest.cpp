#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>

#include "Swiften/Roster/Roster.h"
#include "Swiften/Roster/UnitTest/MockTreeWidget.h"
#include "Swiften/Roster/UnitTest/MockTreeWidgetFactory.h"
#include "Swiften/Roster/UnitTest/MockTreeWidgetItem.h"

using namespace Swift;

class RosterTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RosterTest);
		CPPUNIT_TEST(testGetGroup);
		CPPUNIT_TEST_SUITE_END();

	private:
		Roster *roster_;
		TreeWidget *widget_;
		TreeWidgetFactory *factory_;
		JID jid1_;
		JID jid2_; 
		JID jid3_;

	public:

		RosterTest() : jid1_(JID("a@b.c")), jid2_(JID("b@c.d")), jid3_(JID("c@d.e")) {}

		void setUp() {
			factory_ = new MockTreeWidgetFactory();
			widget_ = factory_->createTreeWidget();
			roster_ = new Roster(widget_, factory_);
		}

		void tearDown() {
			delete roster_;
			//delete widget_;
			delete factory_;
		}

		void testGetGroup() {
			roster_->addContact(jid1_, "Bert", "group1");
			roster_->addContact(jid2_, "Ernie", "group2");
			roster_->addContact(jid3_, "Cookie", "group1");

			CPPUNIT_ASSERT_EQUAL(roster_->getGroup("group1"), roster_->getGroup("group1"));
			CPPUNIT_ASSERT_EQUAL(roster_->getGroup("group2"), roster_->getGroup("group2"));
			CPPUNIT_ASSERT_EQUAL(roster_->getGroup("group3"), roster_->getGroup("group3"));
			CPPUNIT_ASSERT(roster_->getGroup("group1") != roster_->getGroup("group2"));
			CPPUNIT_ASSERT(roster_->getGroup("group2") != roster_->getGroup("group3"));
			CPPUNIT_ASSERT(roster_->getGroup("group3") != roster_->getGroup("group1"));
		}

};
CPPUNIT_TEST_SUITE_REGISTRATION(RosterTest);

