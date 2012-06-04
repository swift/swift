/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/IDN/IDNA.h>

using namespace Swift;

class IDNATest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(IDNATest);
		CPPUNIT_TEST(testGetEncoded);
		CPPUNIT_TEST(testGetEncoded_International);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetEncoded() {
			std::string result = IDNA::getEncoded("www.swift.im");

			CPPUNIT_ASSERT_EQUAL(std::string("www.swift.im"), result);
		}

		void testGetEncoded_International() {
			std::string result = IDNA::getEncoded("www.tron\xc3\x87on.com");

			CPPUNIT_ASSERT_EQUAL(std::string("www.xn--tronon-zua.com"), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(IDNATest);

