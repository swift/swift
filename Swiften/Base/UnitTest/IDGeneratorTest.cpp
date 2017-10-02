/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <set>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/IDGenerator.h>

using namespace Swift;

class IDGeneratorTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(IDGeneratorTest);
        CPPUNIT_TEST(testGenerate);
        CPPUNIT_TEST_SUITE_END();

    public:
        IDGeneratorTest() {}

        void setUp() {
            generatedIDs_.clear();
        }

        void testGenerate() {
            IDGenerator testling;
            for (unsigned int i = 0; i < 26*4; ++i) {
                std::string id = testling.generateID();
                CPPUNIT_ASSERT(generatedIDs_.insert(id).second);
            }
        }

    private:
        std::set<std::string> generatedIDs_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IDGeneratorTest);
