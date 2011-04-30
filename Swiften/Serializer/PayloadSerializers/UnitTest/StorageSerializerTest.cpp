/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/UnitTest/PayloadsSerializer.h>
#include <Swiften/Elements/Storage.h>

using namespace Swift;

class StorageSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(StorageSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerialize_NoNickOrPassword);
		CPPUNIT_TEST_SUITE_END();

	public:
		StorageSerializerTest() {}

		void testSerialize() {
			PayloadsSerializer serializer;
			boost::shared_ptr<Storage> storage(new Storage());
			Storage::Room room;
			room.name = "Council of Oberon";
			room.autoJoin = true;
			room.jid = JID("council@conference.underhill.org");
			room.nick = "Puck";
			room.password = "MyPass";
			storage->addRoom(room);
			Storage::URL url;
			url.name = "Complete Works of Shakespeare";
			url.url = "http://the-tech.mit.edu/Shakespeare/";
			storage->addURL(url);

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<storage xmlns=\"storage:bookmarks\">"
					"<conference "
							"autojoin=\"1\" "
							"jid=\"council@conference.underhill.org\" "
							"name=\"Council of Oberon\">"
						"<nick>Puck</nick>"
						"<password>MyPass</password>"
					"</conference>"
					"<url name=\"Complete Works of Shakespeare\" url=\"http://the-tech.mit.edu/Shakespeare/\"/>"
				"</storage>"), serializer.serialize(storage));
		}

		void testSerialize_NoNickOrPassword() {
			PayloadsSerializer serializer;
			boost::shared_ptr<Storage> storage(new Storage());
			Storage::Room room;
			room.name = "Council of Oberon";
			room.autoJoin = true;
			room.jid = JID("council@conference.underhill.org");
			storage->addRoom(room);

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<storage xmlns=\"storage:bookmarks\">"
					"<conference "
							"autojoin=\"1\" "
							"jid=\"council@conference.underhill.org\" "
							"name=\"Council of Oberon\"/>"
				"</storage>"), serializer.serialize(storage));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StorageSerializerTest);
