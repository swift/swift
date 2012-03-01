/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swift/Controllers/Settings/SettingsProviderHierachy.h>
#include <Swift/Controllers/Settings/DummySettingsProvider.h>
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
	SettingsProviderHierachyTest() : setting1("somekey", 42) {};

	void setUp() {
		bottom = new DummySettingsProvider();
		top = new DummySettingsProvider();
		testling = new SettingsProviderHierachy();
		testling->addProviderToTopOfStack(bottom);
		testling->addProviderToTopOfStack(top);
	}

	void tearDown() {
		delete testling;
		delete top;
		delete bottom;
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
	SettingsProviderHierachy* testling;
	DummySettingsProvider* bottom;
	DummySettingsProvider* top;
	SettingsProvider::Setting<int> setting1;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SettingsProviderHierachyTest);
