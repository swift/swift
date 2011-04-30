/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/PrioritySerializer.h>

using namespace Swift;

class PrioritySerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(PrioritySerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		PrioritySerializerTest() {}

		void testSerialize() {
			PrioritySerializer testling;
			boost::shared_ptr<Priority> priority(new Priority(-113));

			CPPUNIT_ASSERT_EQUAL(std::string("<priority>-113</priority>"), testling.serialize(priority));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PrioritySerializerTest);
