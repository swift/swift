/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/ByteArray.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/StringCodecs/HMACSHA1.h"

using namespace Swift;

class HMACSHA1Test : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(HMACSHA1Test);
		CPPUNIT_TEST(testGetResult);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetResult() {
			ByteArray result(HMACSHA1::getResult("foo", "foobar"));
			CPPUNIT_ASSERT_EQUAL(ByteArray("\xa4\xee\xba\x8e\x63\x3d\x77\x88\x69\xf5\x68\xd0\x5a\x1b\x3d\xc7\x2b\xfd\x4\xdd"), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(HMACSHA1Test);
