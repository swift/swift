#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/PayloadSerializers/StorageSerializer.h"

using namespace Swift;

class StorageSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(StorageSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerialize_NoNickOrPassword);
		CPPUNIT_TEST_SUITE_END();

	public:
		StorageSerializerTest() {}

		void testSerialize() {
			StorageSerializer testling;
			boost::shared_ptr<Storage> storage(new Storage());
			Storage::Conference conference;
			conference.name = "Council of Oberon";
			conference.autoJoin = true;
			conference.jid = JID("council@conference.underhill.org");
			conference.nick = "Puck";
			conference.password = "MyPass";
			storage->addConference(conference);

			CPPUNIT_ASSERT_EQUAL(String(
				"<storage xmlns=\"storage:bookmarks\">"
					"<conference "
							"autojoin=\"1\" "
							"jid=\"council@conference.underhill.org\" "
							"name=\"Council of Oberon\">"
						"<nick>Puck</nick>"
						"<password>MyPass</password>"
					"</conference>"
				"</storage>"), testling.serialize(storage));
		}

		void testSerialize_NoNickOrPassword() {
			StorageSerializer testling;
			boost::shared_ptr<Storage> storage(new Storage());
			Storage::Conference conference;
			conference.name = "Council of Oberon";
			conference.autoJoin = true;
			conference.jid = JID("council@conference.underhill.org");
			storage->addConference(conference);

			CPPUNIT_ASSERT_EQUAL(String(
				"<storage xmlns=\"storage:bookmarks\">"
					"<conference "
							"autojoin=\"1\" "
							"jid=\"council@conference.underhill.org\" "
							"name=\"Council of Oberon\"/>"
				"</storage>"), testling.serialize(storage));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StorageSerializerTest);
