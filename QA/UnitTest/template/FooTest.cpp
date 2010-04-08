/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

using namespace Swift;

class FooTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(FooTest);
		CPPUNIT_TEST(testBar);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
		}

		void tearDown() {
		}

		void testBar() {
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(FooTest);
