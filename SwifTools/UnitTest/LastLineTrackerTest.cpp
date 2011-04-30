/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <SwifTools/LastLineTracker.h>

using namespace Swift;

class LastLineTrackerTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(LastLineTrackerTest);
	CPPUNIT_TEST(testFocusNormal);
	CPPUNIT_TEST(testFocusOut);
	CPPUNIT_TEST(testFocusOtherTab);
	CPPUNIT_TEST(testRepeatedFocusOut);
	CPPUNIT_TEST(testRepeatedFocusIn);
	CPPUNIT_TEST_SUITE_END();
	public:
	LastLineTrackerTest () {
	};
	void testFocusNormal() {
		LastLineTracker testling;
		testling.setHasFocus(true);
		CPPUNIT_ASSERT_EQUAL(false, testling.getShouldMoveLastLine());
	}
	void testFocusOut() {
		LastLineTracker testling;
		testling.setHasFocus(false);
		CPPUNIT_ASSERT_EQUAL(true, testling.getShouldMoveLastLine());
		CPPUNIT_ASSERT_EQUAL(false, testling.getShouldMoveLastLine());
		CPPUNIT_ASSERT_EQUAL(false, testling.getShouldMoveLastLine());
	}
	void testFocusOtherTab() {
		LastLineTracker testling;
		testling.setHasFocus(true);
		testling.setHasFocus(false);
		CPPUNIT_ASSERT_EQUAL(true, testling.getShouldMoveLastLine());
		CPPUNIT_ASSERT_EQUAL(false, testling.getShouldMoveLastLine());
	}

	void testRepeatedFocusOut() {
		LastLineTracker testling;
		testling.setHasFocus(true);
		CPPUNIT_ASSERT_EQUAL(false, testling.getShouldMoveLastLine());
		testling.setHasFocus(false);
		CPPUNIT_ASSERT_EQUAL(true, testling.getShouldMoveLastLine());
		testling.setHasFocus(false);
		CPPUNIT_ASSERT_EQUAL(false, testling.getShouldMoveLastLine());
	}
	void testRepeatedFocusIn() {
		LastLineTracker testling;
		testling.setHasFocus(false);
		CPPUNIT_ASSERT_EQUAL(true, testling.getShouldMoveLastLine());
		testling.setHasFocus(true);
		testling.setHasFocus(false);
		CPPUNIT_ASSERT_EQUAL(true, testling.getShouldMoveLastLine());
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LastLineTrackerTest);
