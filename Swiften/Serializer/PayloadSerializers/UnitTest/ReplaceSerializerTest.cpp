/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/ReplaceSerializer.h>

using namespace Swift;

class ReplaceSerializerTest: public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ReplaceSerializerTest);
	CPPUNIT_TEST(testSerialize);
	CPPUNIT_TEST_SUITE_END();

	public:
		ReplaceSerializerTest() {}

		void testSerialize() {
			ReplaceSerializer testling;
			boost::shared_ptr<Replace> replace(new Replace());
			replace->setID("bad1");
			CPPUNIT_ASSERT_EQUAL(std::string("<replace id = 'bad1' xmlns='http://swift.im/protocol/replace'/>"), testling.serialize(replace));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ReplaceSerializerTest);
