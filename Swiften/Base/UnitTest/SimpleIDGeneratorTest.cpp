/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <set>

#include "Swiften/Base/SimpleIDGenerator.h"

using namespace Swift;

class SimpleIDGeneratorTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SimpleIDGeneratorTest);
		CPPUNIT_TEST(testGenerate);
		CPPUNIT_TEST_SUITE_END();

	public:
		SimpleIDGeneratorTest() {}

		void setUp() {
			generatedIDs_.clear();
		}

		void testGenerate() {
			SimpleIDGenerator testling;
			for (unsigned int i = 0; i < 26*4; ++i) {
				std::string id = testling.generateID();
				CPPUNIT_ASSERT(generatedIDs_.insert(id).second);
			}
		}
	
	private:
		std::set<std::string> generatedIDs_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SimpleIDGeneratorTest);
