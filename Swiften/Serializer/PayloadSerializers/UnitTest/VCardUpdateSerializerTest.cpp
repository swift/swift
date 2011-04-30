/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/VCardUpdateSerializer.h>

using namespace Swift;

class VCardUpdateSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(VCardUpdateSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		VCardUpdateSerializerTest() {}

		void testSerialize() {
			VCardUpdateSerializer testling;
			boost::shared_ptr<VCardUpdate> update(new VCardUpdate());
			update->setPhotoHash("sha1-hash-of-image");

			std::string expectedResult = 
				"<x xmlns=\"vcard-temp:x:update\">"
					"<photo>sha1-hash-of-image</photo>"
				"</x>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(update));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardUpdateSerializerTest);
