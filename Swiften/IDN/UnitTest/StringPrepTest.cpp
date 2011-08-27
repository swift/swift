/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/IDN/StringPrep.h"

using namespace Swift;

class StringPrepTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(StringPrepTest);
		CPPUNIT_TEST(testStringPrep);
		CPPUNIT_TEST(testStringPrep_Empty);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testStringPrep() {
			std::string result = StringPrep::getPrepared("tron\xc3\x87on", StringPrep::NamePrep);

			CPPUNIT_ASSERT_EQUAL(std::string("tron\xc3\xa7on"), result);
		}

		void testStringPrep_Empty() {
			CPPUNIT_ASSERT_EQUAL(std::string(""), StringPrep::getPrepared("", StringPrep::NamePrep));
			CPPUNIT_ASSERT_EQUAL(std::string(""), StringPrep::getPrepared("", StringPrep::XMPPNodePrep));
			CPPUNIT_ASSERT_EQUAL(std::string(""), StringPrep::getPrepared("", StringPrep::XMPPResourcePrep));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StringPrepTest);
