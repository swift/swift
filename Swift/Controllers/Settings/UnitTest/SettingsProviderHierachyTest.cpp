/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swift/Controllers/Settings/DummySettingsProvider.h>
#include <Swift/Controllers/Settings/SettingsProviderHierachy.h>
#include <Swift/Controllers/Settings/XMLSettingsProvider.h>

using namespace Swift;
using namespace std;

class SettingsProviderHierachyTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SettingsProviderHierachyTest);
    CPPUNIT_TEST(testEmpty);
    CPPUNIT_TEST(testTop);
    CPPUNIT_TEST(testBottom);
    CPPUNIT_TEST(testBoth);
    CPPUNIT_TEST(testTopDefault);
    CPPUNIT_TEST(testBottomOverrides);
    CPPUNIT_TEST(testFinal);
    CPPUNIT_TEST_SUITE_END();

public:
    SettingsProviderHierachyTest() : setting1("somekey", 42) {}

    void setUp() {
        bottom = std::unique_ptr<DummySettingsProvider>(new DummySettingsProvider());
        top = std::unique_ptr<DummySettingsProvider>(new DummySettingsProvider());
        testling = std::unique_ptr<SettingsProviderHierachy>(new SettingsProviderHierachy());
        testling->addProviderToTopOfStack(bottom.get());
        testling->addProviderToTopOfStack(top.get());
    }

    void testEmpty() {
        CPPUNIT_ASSERT_EQUAL(42, testling->getSetting(setting1));
    }

    void testTop() {
        top->storeSetting(setting1, 37);
        CPPUNIT_ASSERT_EQUAL(37, testling->getSetting(setting1));
    }

    void testBottom() {
        bottom->storeSetting(setting1, 17);
        CPPUNIT_ASSERT_EQUAL(17, testling->getSetting(setting1));
    }

    void testBoth() {
        bottom->storeSetting(setting1, 17);
        top->storeSetting(setting1, 37);
        CPPUNIT_ASSERT_EQUAL(37, testling->getSetting(setting1));
    }

    void testTopDefault() {
        bottom->storeSetting(setting1, 17);
        top->storeSetting(setting1, 42);
        CPPUNIT_ASSERT_EQUAL(42, testling->getSetting(setting1));
    }

    void testBottomOverrides() {
        bottom->storeSetting(setting1, 17);
        bottom->setFinal(setting1.getKey());
        top->storeSetting(setting1, 5);
        CPPUNIT_ASSERT_EQUAL(17, testling->getSetting(setting1));
    }

    void testFinal() {
        bottom->storeSetting(setting1, 17);
        bottom->setFinal(setting1.getKey());
        testling->storeSetting(setting1, 5);
        CPPUNIT_ASSERT_EQUAL(17, testling->getSetting(setting1));
    }
private:
    std::unique_ptr<SettingsProviderHierachy> testling;
    std::unique_ptr<DummySettingsProvider> bottom;
    std::unique_ptr<DummySettingsProvider> top;
    SettingsProvider::Setting<int> setting1;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SettingsProviderHierachyTest);
