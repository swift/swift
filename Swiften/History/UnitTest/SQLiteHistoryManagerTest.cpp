/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/History/SQLiteHistoryManager.h>

using namespace Swift;

class SQLiteHistoryManagerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SQLiteHistoryManagerTest);
		//CPPUNIT_TEST(testAddMessage);
		//CPPUNIT_TEST(testAddMessage_TwoMessages);
		//CPPUNIT_TEST(testGetIDForJID_SameJID);
		//CPPUNIT_TEST(testGetIDForJID_DifferentJIDs);
		//CPPUNIT_TEST(getJIDFromID);
		//CPPUNIT_TEST(getJIDFromID_UnexistingID);
		//CPPUNIT_TEST(getIDFromJID);
		//CPPUNIT_TEST(getIDFromJID_UnexistingJID);
		//CPPUNIT_TEST_SUITE_END();

	public:
		SQLiteHistoryManagerTest() {}

		void setUp() {
		}

		void tearDown() {
		}

		void testAddMessage() {
			boost::shared_ptr<SQLiteHistoryManager> testling(createHistoryManager());
			HistoryMessage testMessage("Test", JID("foo@bar.com"), JID("fum@baz.org"), boost::posix_time::time_from_string("1980-01-21 22:03"));
			testling->addMessage(testMessage);

			std::vector<HistoryMessage> messages = testling->getMessages();
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(messages.size()));
			CPPUNIT_ASSERT(testMessage == messages[0]);
		}

		void testAddMessage_TwoMessages() {
			boost::shared_ptr<SQLiteHistoryManager> testling(createHistoryManager());
			HistoryMessage testMessage1("Test1", JID("foo@bar.com"), JID("fum@baz.org"), boost::posix_time::time_from_string("1980-01-21 22:03"));
			testling->addMessage(testMessage1);
			HistoryMessage testMessage2("Test2", JID("fum@baz.org"), JID("foo@bar.com"), boost::posix_time::time_from_string("1975-03-09 22:04"));
			testling->addMessage(testMessage2);

			std::vector<HistoryMessage> messages = testling->getMessages();
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(messages.size()));
			CPPUNIT_ASSERT(testMessage1 == messages[0]);
			CPPUNIT_ASSERT(testMessage2 == messages[1]);
		}

		void testGetIDForJID_SameJID() {
			boost::shared_ptr<SQLiteHistoryManager> testling(createHistoryManager());
			int id1 = testling->getIDForJID(JID("foo@bar.com"));
			int id2 = testling->getIDForJID(JID("foo@bar.com"));

			CPPUNIT_ASSERT_EQUAL(id1, id2);
		}

		void testGetIDForJID_DifferentJIDs() {
			boost::shared_ptr<SQLiteHistoryManager> testling(createHistoryManager());
			int id1 = testling->getIDForJID(JID("foo@bar.com"));
			int id2 = testling->getIDForJID(JID("foo@baz.com"));

			CPPUNIT_ASSERT(id1 != id2);
		}

		void getJIDFromID() {
			boost::shared_ptr<SQLiteHistoryManager> testling(createHistoryManager());
			int id = testling->addJID(JID("foo@bar.com"));

			boost::optional<JID> result(testling->getJIDFromID(id));
			CPPUNIT_ASSERT(result);
			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com"), *result);
		}

		void getJIDFromID_UnexistingID() {
			boost::shared_ptr<SQLiteHistoryManager> testling(createHistoryManager());

			boost::optional<JID> result(testling->getJIDFromID(1));

			CPPUNIT_ASSERT(!result);
		}

		void getIDFromJID() {
			boost::shared_ptr<SQLiteHistoryManager> testling(createHistoryManager());
			int id = testling->addJID(JID("foo@bar.com"));

			boost::optional<int> result(testling->getIDFromJID(JID("foo@bar.com")));
			CPPUNIT_ASSERT(result);
			CPPUNIT_ASSERT_EQUAL(id, *result);
		}

		void getIDFromJID_UnexistingJID() {
			boost::shared_ptr<SQLiteHistoryManager> testling(createHistoryManager());

			boost::optional<int> result(testling->getIDFromJID(JID("foo@bar.com")));

			CPPUNIT_ASSERT(!result);
		}

	private:
		SQLiteHistoryManager* createHistoryManager() {
			return new SQLiteHistoryManager(":memory:");
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SQLiteHistoryManagerTest);
