/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Roster/TableRoster.h>

std::ostream& operator<<(std::ostream& os, const Swift::TableRoster::Index& i) {
	os << "(" << i.section << ", " << i.row << ")";
	return os;
}

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

#include <Swiften/Network/DummyTimerFactory.h>
#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Roster/SetPresence.h>

using namespace Swift;

class TableRosterTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(TableRosterTest);
		CPPUNIT_TEST(testAddContact_EmptyRoster);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			timerFactory = new DummyTimerFactory();
			roster = new Roster();
			jid1 = JID("jid1@example.com");
			jid2 = JID("jid2@example.com");
		}

		void tearDown() {
			delete roster;
			delete timerFactory;
		}

		void testAddContact_EmptyRoster() {
			/*
			boost::shared_ptr<TableRoster> tableRoster(createTestling());

			addContact(jid1, "1", "group1");

			CPPUNIT_ASSERT_EQUAL(4, static_cast<int>(events.size()));
			CPPUNIT_ASSERT(boost::get<BeginUpdatesEvent>(&events[0]));
			CPPUNIT_ASSERT(boost::get<SectionsInsertedEvent>(&events[1]));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(boost::get<SectionsInsertedEvent>(events[1]).sections.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(boost::get<SectionsInsertedEvent>(events[1]).sections[0]));
			CPPUNIT_ASSERT(boost::get<RowsInsertedEvent>(&events[2]));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(boost::get<RowsInsertedEvent>(events[2]).rows.size()));
			CPPUNIT_ASSERT_EQUAL(TableRoster::Index(0, 0), boost::get<RowsInsertedEvent>(events[2]).rows[0]);
			CPPUNIT_ASSERT(boost::get<EndUpdatesEvent>(&events[3]));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(tableRoster->getNumberOfSections()));
			CPPUNIT_ASSERT_EQUAL(std::string("group1"), tableRoster->getSectionTitle(0));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(tableRoster->getNumberOfRowsInSection(0)));
			CPPUNIT_ASSERT_EQUAL(jid1, tableRoster->getItem(TableRoster::Index(0, 0)).jid);
			*/
		}

	private:
		void addContact(const JID& jid, const std::string& name, const std::string& group) {
			roster->addContact(jid, JID(), name, group, "");
		}

		TableRoster* createTestling() {
			TableRoster* result = new TableRoster(roster, timerFactory, 10);
			result->onUpdate.connect(boost::bind(&TableRosterTest::handleUpdate, this, _1));
			return result;
		}

		void handleUpdate(const TableRoster::Update& update) {
			updates.push_back(update);
		}

	private:
		DummyTimerFactory* timerFactory;
		Roster* roster;
		JID jid1;
		JID jid2;
		std::vector<TableRoster::Update> updates;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TableRosterTest);

