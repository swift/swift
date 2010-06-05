/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "SwifTools/TabComplete.h"

using namespace Swift;

class TabCompleteTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TabCompleteTest);
	CPPUNIT_TEST(testEmpty);
	CPPUNIT_TEST(testNoMatch);
	CPPUNIT_TEST(testOneMatch);
	CPPUNIT_TEST(testTwoMatch);
	CPPUNIT_TEST(testChangeMatch);
	CPPUNIT_TEST(testRemoveDuringComplete);
	CPPUNIT_TEST(testAddDuringComplete);
	CPPUNIT_TEST_SUITE_END();
	
public:
	TabCompleteTest() {};
	
	void setUp() {
		completer_ = TabComplete();
	}
	
	void testEmpty() {
		String blah("Blah");
		CPPUNIT_ASSERT_EQUAL(
			blah,
			completer_.completeWord(blah));
		CPPUNIT_ASSERT_EQUAL(
			blah,
			completer_.completeWord(blah));
	}

	void testNoMatch() {
		completer_.addWord("Bleh");
		String blah("Blah");
		CPPUNIT_ASSERT_EQUAL(
			blah,
			completer_.completeWord(blah));
		CPPUNIT_ASSERT_EQUAL(
			blah,
			completer_.completeWord(blah));
	}

	void testOneMatch() {
		String short1("Bl");
		String long1("Blehling");
		completer_.addWord(long1);
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(short1));
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(long1));
	}
	
	void testTwoMatch() {
		String short1("Hur");
		String long1("Hurgle");
		String long2("Hurdler");
		completer_.addWord(long1);
		completer_.addWord("Blah");
		completer_.addWord(long2);
		completer_.addWord("Bleh");
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(short1));
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(long1));
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(long2));
	}

	void testChangeMatch() {
		String short1("Hur");
		String short2("Rub");
		String long1("Hurgle");
		String long2("Rubbish");
		completer_.addWord(long2);
		completer_.addWord("Blah");
		completer_.addWord(long1);
		completer_.addWord("Bleh");
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(short1));
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(short2));
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(long2));
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(short1));
	}

	void testRemoveDuringComplete() {
		String short1("Kev");
		String long1("Kevin");
		String long2("Kevlar");
		completer_.addWord(long1);
		completer_.addWord("Blah");
		completer_.addWord(long2);
		completer_.addWord("Bleh");
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(short1));
		completer_.removeWord(long1);
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(long1));
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(long2));
	}

	void testAddDuringComplete() {
		String short1("Rem");
		String long1("Remko");
		String long2("Remove");
		String long3("Remedial");
		completer_.addWord(long1);
		completer_.addWord("Blah");
		completer_.addWord(long2);
		completer_.addWord("Bleh");
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(short1));
		completer_.addWord(long3);
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(long1));
		CPPUNIT_ASSERT_EQUAL(
			long3,
			completer_.completeWord(long2));
	}

private:
	TabComplete completer_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TabCompleteTest);
