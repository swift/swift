/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/StatusSerializer.h>

using namespace Swift;

class StatusSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StatusSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		StatusSerializerTest() {}

		void testSerialize() {
			StatusSerializer testling;
			boost::shared_ptr<Status> status(new Status("I am away"));

			CPPUNIT_ASSERT_EQUAL(std::string("<status>I am away</status>"), testling.serialize(status));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StatusSerializerTest);
