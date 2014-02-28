/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/smart_ptr/make_shared.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Serializer/PayloadSerializers/MAMArchivedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

class MAMArchivedSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(MAMArchivedSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize() {
			MAMArchivedSerializer serializer;

			boost::shared_ptr<MAMArchived> archived(boost::make_shared<MAMArchived>());
			archived->setBy("juliet@capulet.lit");
			archived->setID("28482-98726-73623");

			std::string expectedResult =
				"<archived by=\"juliet@capulet.lit\" id=\"28482-98726-73623\" xmlns=\"urn:xmpp:mam:0\"/>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(archived));
		}

	private:
		FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MAMArchivedSerializerTest);
