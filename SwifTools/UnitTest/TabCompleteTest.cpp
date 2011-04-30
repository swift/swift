/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <SwifTools/TabComplete.h>

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
	CPPUNIT_TEST(testSwiftRoomSample);
	CPPUNIT_TEST_SUITE_END();
	
public:
	TabCompleteTest() {};
	
	void setUp() {
		completer_ = TabComplete();
	}
	
	void testEmpty() {
		std::string blah("Blah");
		CPPUNIT_ASSERT_EQUAL(
			blah,
			completer_.completeWord(blah));
		CPPUNIT_ASSERT_EQUAL(
			blah,
			completer_.completeWord(blah));
	}

	void testNoMatch() {
		completer_.addWord("Bleh");
		std::string blah("Blah");
		CPPUNIT_ASSERT_EQUAL(
			blah,
			completer_.completeWord(blah));
		CPPUNIT_ASSERT_EQUAL(
			blah,
			completer_.completeWord(blah));
	}

	void testOneMatch() {
		std::string short1("Bl");
		std::string long1("Blehling");
		completer_.addWord(long1);
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(short1));
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(long1));
	}
	
	void testTwoMatch() {
		std::string short1("Hur");
		std::string long1("Hurgle");
		std::string long2("Hurdler");
		completer_.addWord(long1);
		completer_.addWord("Blah");
		completer_.addWord(long2);
		completer_.addWord("Bleh");
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(short1));
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(long2));
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(long1));
	}

	void testChangeMatch() {
		std::string short1("Hur");
		std::string short2("Rub");
		std::string long1("Hurgle");
		std::string long2("Rubbish");
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
		std::string short1("Kev");
		std::string long1("Kevin");
		std::string long2("Kevlar");
		completer_.addWord(long1);
		completer_.addWord("Blah");
		completer_.addWord(long2);
		completer_.addWord("Bleh");
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(short1));
		completer_.removeWord(long2);
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(long2));
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(long1));
	}

	void testAddDuringComplete() {
		std::string short1("Rem");
		std::string long1("Remko");
		std::string long2("Remove");
		std::string long3("Remedial");
		completer_.addWord(long1);
		completer_.addWord("Blah");
		completer_.addWord(long2);
		completer_.addWord("Bleh");
		CPPUNIT_ASSERT_EQUAL(
			long2,
			completer_.completeWord(short1));
		completer_.addWord(long3);
		CPPUNIT_ASSERT_EQUAL(
			long1,
			completer_.completeWord(long2));
		CPPUNIT_ASSERT_EQUAL(
			long3,
			completer_.completeWord(long1));
	}

	void testSwiftRoomSample() {
		std::string t("t");
		std::string Anpan("Anpan");
		std::string cdubouloz("cdubouloz");
		std::string Tobias("Tobias");
		std::string Zash("Zash");
		std::string lastsky("lastsky");
		std::string Steve("Steve Kille");
		std::string Flo("Flo");
		std::string Test("Test");
		std::string test("test");
		completer_.addWord(Anpan);
		completer_.addWord(cdubouloz);
		completer_.addWord(Tobias);
		completer_.addWord(lastsky);
		completer_.addWord(Steve);
		completer_.addWord(Flo);
		completer_.addWord(Zash);

		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(Tobias));

		completer_.addWord(Test);

		CPPUNIT_ASSERT_EQUAL(
			Test,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(Test));
		CPPUNIT_ASSERT_EQUAL(
			Test,
			completer_.completeWord(Tobias));
		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(Test));

		completer_.addWord(Zash);
		completer_.addWord(Zash);
		completer_.addWord(Zash);
		completer_.addWord(Zash);

		completer_.removeWord(Test);

		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(Tobias));

		completer_.addWord(test);

		completer_.addWord(Zash);
		completer_.addWord(Zash);
		completer_.addWord(Zash);
		completer_.addWord(Zash);


		CPPUNIT_ASSERT_EQUAL(
			test,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(test));
		CPPUNIT_ASSERT_EQUAL(
			test,
			completer_.completeWord(Tobias));
		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(test));

		completer_.removeWord(test);

		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(Tobias));

		completer_.removeWord(Tobias);
		CPPUNIT_ASSERT_EQUAL(
			t,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			t,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			t,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			t,
			completer_.completeWord(t));

		completer_.addWord(Tobias);

		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(t));
		CPPUNIT_ASSERT_EQUAL(
			Tobias,
			completer_.completeWord(Tobias));

	}



private:
	TabComplete completer_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TabCompleteTest);
