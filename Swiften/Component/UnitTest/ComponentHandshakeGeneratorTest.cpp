/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Component/ComponentHandshakeGenerator.h>

using namespace Swift;

class ComponentHandshakeGeneratorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ComponentHandshakeGeneratorTest);
		CPPUNIT_TEST(testGetHandshake);
		CPPUNIT_TEST(testGetHandshake_SpecialChars);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetHandshake() {
			std::string result = ComponentHandshakeGenerator::getHandshake("myid", "mysecret");
			CPPUNIT_ASSERT_EQUAL(std::string("4011cd31f9b99ac089a0cd7ce297da7323fa2525"), result);
		}

		void testGetHandshake_SpecialChars() {
			std::string result = ComponentHandshakeGenerator::getHandshake("&<", ">'\"");
			CPPUNIT_ASSERT_EQUAL(std::string("33631b3e0aaeb2a11c4994c917919324028873fe"), result);
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION(ComponentHandshakeGeneratorTest);
