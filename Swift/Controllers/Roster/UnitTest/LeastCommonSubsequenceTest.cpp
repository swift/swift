/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/assign/list_of.hpp>
#include <functional>

#include <QA/Checker/IO.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <Swift/Controllers/Roster/LeastCommonSubsequence.h>

using namespace Swift;

struct IsBOrC {
	bool operator()(char c, char c2) const {
		CPPUNIT_ASSERT_EQUAL(c, c2);
		return c == 'b' || c == 'c';
	}
};

struct IsXOrY {
	bool operator()(char c, char c2) const {
		CPPUNIT_ASSERT_EQUAL(c, c2);
		return c == 'x' || c == 'y';
	}
};

struct IsArizonaOrNewJersey {
	bool operator()(const std::string& s, const std::string& s2) const {
		CPPUNIT_ASSERT_EQUAL(s, s2);
		return s == "Arizona" || s == "New Jersey";
	}
};

class LeastCommonSubsequenceTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LeastCommonSubsequenceTest);
		CPPUNIT_TEST(testComputeLeastCommonSubsequenceMatrix_1);
		CPPUNIT_TEST(testComputeLeastCommonSubsequenceMatrix_2);
		CPPUNIT_TEST(testComputeLeastCommonSubsequenceMatrix_Sequence1Empty);
		CPPUNIT_TEST(testComputeLeastCommonSubsequenceMatrix_Sequence2Empty);
		CPPUNIT_TEST(testComputeLeastCommonSubsequenceMatrix_BothSequencesEmpty);
		CPPUNIT_TEST(testComputeLeastCommonSubsequenceMatrix_NoCommonSequence);
		CPPUNIT_TEST(testComputeLeastCommonSubsequenceMatrix_SameSequences);
		CPPUNIT_TEST(testComputeIndexDiff_1);
		CPPUNIT_TEST(testComputeIndexDiff_2);
		CPPUNIT_TEST(testComputeIndexDiff_Sequence1Empty);
		CPPUNIT_TEST(testComputeIndexDiff_Sequence2Empty);
		CPPUNIT_TEST(testComputeIndexDiff_BothSequencesEmpty);
		CPPUNIT_TEST(testComputeIndexDiff_NoCommonSequence);
		CPPUNIT_TEST(testComputeIndexDiff_SameSequences);
		CPPUNIT_TEST(testComputeIndexDiff_CommonPrefixAndSuffix);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testComputeLeastCommonSubsequenceMatrix_1() {
			std::vector<char> x = boost::assign::list_of('x')('m')('j')('y')('a')('u')('z');
			std::vector<char> y = boost::assign::list_of('m')('z')('j')('a')('w')('x')('u');

			std::vector<int> result;
			Detail::computeLeastCommonSubsequenceMatrix<std::vector<char>::const_iterator, std::vector<char>::const_iterator, int, std::equal_to<char> >(x.begin(), x.end(), y.begin(), y.end(), result);

			std::vector<int> expected = boost::assign::list_of
				(0)(0)(0)(0)(0)(0)(0)(0)
				(0)(0)(1)(1)(1)(1)(1)(1)
				(0)(0)(1)(1)(1)(1)(1)(2)
				(0)(0)(1)(2)(2)(2)(2)(2)
				(0)(0)(1)(2)(2)(3)(3)(3)
				(0)(0)(1)(2)(2)(3)(3)(3)
				(0)(1)(1)(2)(2)(3)(3)(3)
				(0)(1)(1)(2)(2)(3)(4)(4);
			CPPUNIT_ASSERT_EQUAL(expected, result);
		}

		void testComputeLeastCommonSubsequenceMatrix_2() {
			std::vector<char> x = boost::assign::list_of('x')('x')('x')('m')('j')('y')('a')('u')('z');
			std::vector<char> y = boost::assign::list_of('m')('z')('j')('a')('w')('x')('u');

			std::vector<int> result;
			Detail::computeLeastCommonSubsequenceMatrix<std::vector<char>::const_iterator, std::vector<char>::const_iterator, int, std::equal_to<char> >(x.begin(), x.end(), y.begin(), y.end(), result);

			std::vector<int> expected = boost::assign::list_of
				(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)
				(0)(0)(0)(0)(1)(1)(1)(1)(1)(1)
				(0)(0)(0)(0)(1)(1)(1)(1)(1)(2)
				(0)(0)(0)(0)(1)(2)(2)(2)(2)(2)
				(0)(0)(0)(0)(1)(2)(2)(3)(3)(3)
				(0)(0)(0)(0)(1)(2)(2)(3)(3)(3)
				(0)(1)(1)(1)(1)(2)(2)(3)(3)(3)
				(0)(1)(1)(1)(1)(2)(2)(3)(4)(4);
			CPPUNIT_ASSERT_EQUAL(expected, result);
		}

		void testComputeLeastCommonSubsequenceMatrix_Sequence1Empty() {
			std::vector<char> x;
			std::vector<char> y = boost::assign::list_of('a')('b')('c');

			std::vector<int> result;
			Detail::computeLeastCommonSubsequenceMatrix<std::vector<char>::const_iterator, std::vector<char>::const_iterator, int, std::equal_to<char> >(x.begin(), x.end(), y.begin(), y.end(), result);

			std::vector<int> expected = boost::assign::list_of
				(0)
				(0)
				(0)
				(0);
			CPPUNIT_ASSERT_EQUAL(expected, result);
		}

		void testComputeLeastCommonSubsequenceMatrix_Sequence2Empty() {
			std::vector<char> x = boost::assign::list_of('a')('b')('c');
			std::vector<char> y;

			std::vector<int> result;
			Detail::computeLeastCommonSubsequenceMatrix<std::vector<char>::const_iterator, std::vector<char>::const_iterator, int, std::equal_to<char> >(x.begin(), x.end(), y.begin(), y.end(), result);

			std::vector<int> expected = boost::assign::list_of
				(0)(0)(0)(0);
			CPPUNIT_ASSERT_EQUAL(expected, result);
		}

		void testComputeLeastCommonSubsequenceMatrix_BothSequencesEmpty() {
			std::vector<char> x;
			std::vector<char> y;

			std::vector<int> result;
			Detail::computeLeastCommonSubsequenceMatrix<std::vector<char>::const_iterator, std::vector<char>::const_iterator, int, std::equal_to<char> >(x.begin(), x.end(), y.begin(), y.end(), result);

			std::vector<int> expected = boost::assign::list_of(0);
			CPPUNIT_ASSERT_EQUAL(expected, result);
		}

		void testComputeLeastCommonSubsequenceMatrix_NoCommonSequence() {
			std::vector<char> x = boost::assign::list_of('a')('b')('c');
			std::vector<char> y = boost::assign::list_of('d')('e')('f')('g');

			std::vector<int> result;
			Detail::computeLeastCommonSubsequenceMatrix<std::vector<char>::const_iterator, std::vector<char>::const_iterator, int, std::equal_to<char> >(x.begin(), x.end(), y.begin(), y.end(), result);

			std::vector<int> expected = boost::assign::list_of
				(0)(0)(0)(0)
				(0)(0)(0)(0)
				(0)(0)(0)(0)
				(0)(0)(0)(0)
				(0)(0)(0)(0);
			CPPUNIT_ASSERT_EQUAL(expected, result);
		}

		void testComputeLeastCommonSubsequenceMatrix_SameSequences() {
			std::vector<char> x = boost::assign::list_of('a')('b')('c');
			std::vector<char> y = boost::assign::list_of('a')('b')('c');

			std::vector<int> result;
			Detail::computeLeastCommonSubsequenceMatrix<std::vector<char>::const_iterator, std::vector<char>::const_iterator, int, std::equal_to<char> >(x.begin(), x.end(), y.begin(), y.end(), result);

			std::vector<int> expected = boost::assign::list_of
				(0)(0)(0)(0)
				(0)(1)(1)(1)
				(0)(1)(2)(2)
				(0)(1)(2)(3);
			CPPUNIT_ASSERT_EQUAL(expected, result);
		}

		void testComputeIndexDiff_1() {
			std::vector<std::string> x = boost::assign::list_of("Arizona")("California")("Delaware")("New Jersey")("Washington");
			std::vector<std::string> y = boost::assign::list_of("Alaska")("Arizona")("California")("Georgia")("New Jersey")("Virginia");

			std::vector<size_t> updates;
			std::vector<size_t> postUpdates;
			std::vector<size_t> removes;
			std::vector<size_t> inserts;
			computeIndexDiff<std::string, std::equal_to<std::string>, IsArizonaOrNewJersey >(x, y, updates, postUpdates, removes, inserts);
			
			std::vector<size_t> expectedUpdates = boost::assign::list_of(3)(0);
			std::vector<size_t> expectedPostUpdates = boost::assign::list_of(4)(1);
			std::vector<size_t> expectedRemoves = boost::assign::list_of(4)(2);
			std::vector<size_t> expectedInserts = boost::assign::list_of(5)(3)(0);
			CPPUNIT_ASSERT_EQUAL(expectedUpdates, updates);
			CPPUNIT_ASSERT_EQUAL(expectedPostUpdates, postUpdates);
			CPPUNIT_ASSERT_EQUAL(expectedRemoves, removes);
			CPPUNIT_ASSERT_EQUAL(expectedInserts, inserts);
		}

		void testComputeIndexDiff_2() {
			std::vector<char> x = boost::assign::list_of('x')('y');
			std::vector<char> y = boost::assign::list_of('x');

			std::vector<size_t> updates;
			std::vector<size_t> postUpdates;
			std::vector<size_t> removes;
			std::vector<size_t> inserts;
			computeIndexDiff<char, std::equal_to<char>, IsBOrC >(x, y, updates, postUpdates, removes, inserts);
			
			std::vector<size_t> expectedRemoves = boost::assign::list_of(1);
			CPPUNIT_ASSERT(updates.empty());
			CPPUNIT_ASSERT(postUpdates.empty());
			CPPUNIT_ASSERT(inserts.empty());
			CPPUNIT_ASSERT_EQUAL(expectedRemoves, removes);
		}

		void testComputeIndexDiff_Sequence1Empty() {
			std::vector<char> x;
			std::vector<char> y = boost::assign::list_of('a')('b')('c');

			std::vector<size_t> updates;
			std::vector<size_t> postUpdates;
			std::vector<size_t> removes;
			std::vector<size_t> inserts;
			computeIndexDiff<char, std::equal_to<char>, IsBOrC >(x, y, updates, postUpdates, removes, inserts);
			
			std::vector<size_t> expectedInserts = boost::assign::list_of(2)(1)(0);
			CPPUNIT_ASSERT(updates.empty());
			CPPUNIT_ASSERT(postUpdates.empty());
			CPPUNIT_ASSERT(removes.empty());
			CPPUNIT_ASSERT_EQUAL(expectedInserts, inserts);
		}

		void testComputeIndexDiff_Sequence2Empty() {
			std::vector<char> x = boost::assign::list_of('a')('b')('c');
			std::vector<char> y;

			std::vector<size_t> updates;
			std::vector<size_t> postUpdates;
			std::vector<size_t> removes;
			std::vector<size_t> inserts;
			computeIndexDiff<char, std::equal_to<char>, IsBOrC >(x, y, updates, postUpdates, removes, inserts);
			
			std::vector<size_t> expectedRemoves = boost::assign::list_of(2)(1)(0);
			CPPUNIT_ASSERT(updates.empty());
			CPPUNIT_ASSERT(postUpdates.empty());
			CPPUNIT_ASSERT_EQUAL(expectedRemoves, removes);
			CPPUNIT_ASSERT(inserts.empty());
		}

		void testComputeIndexDiff_BothSequencesEmpty() {
			std::vector<char> x;
			std::vector<char> y;

			std::vector<size_t> updates;
			std::vector<size_t> postUpdates;
			std::vector<size_t> removes;
			std::vector<size_t> inserts;
			computeIndexDiff<char, std::equal_to<char>, IsBOrC >(x, y, updates, postUpdates, removes, inserts);
			
			CPPUNIT_ASSERT(updates.empty());
			CPPUNIT_ASSERT(postUpdates.empty());
			CPPUNIT_ASSERT(removes.empty());
			CPPUNIT_ASSERT(inserts.empty());
		}

		void testComputeIndexDiff_NoCommonSequence() {
			std::vector<char> x = boost::assign::list_of('a')('b')('c');
			std::vector<char> y = boost::assign::list_of('d')('e')('f')('g');

			std::vector<size_t> updates;
			std::vector<size_t> postUpdates;
			std::vector<size_t> removes;
			std::vector<size_t> inserts;
			computeIndexDiff<char, std::equal_to<char>, IsBOrC >(x, y, updates, postUpdates, removes, inserts);
			
			std::vector<size_t> expectedRemoves = boost::assign::list_of(2)(1)(0);
			std::vector<size_t> expectedInserts = boost::assign::list_of(3)(2)(1)(0);
			CPPUNIT_ASSERT(updates.empty());
			CPPUNIT_ASSERT(postUpdates.empty());
			CPPUNIT_ASSERT_EQUAL(expectedRemoves, removes);
			CPPUNIT_ASSERT_EQUAL(expectedInserts, inserts);
		}

		void testComputeIndexDiff_SameSequences() {
			std::vector<char> x = boost::assign::list_of('a')('b')('c');
			std::vector<char> y = boost::assign::list_of('a')('b')('c');

			std::vector<size_t> updates;
			std::vector<size_t> postUpdates;
			std::vector<size_t> removes;
			std::vector<size_t> inserts;
			computeIndexDiff<char, std::equal_to<char>, IsBOrC >(x, y, updates, postUpdates, removes, inserts);
			
			std::vector<size_t> expectedUpdates = boost::assign::list_of(1)(2);
			CPPUNIT_ASSERT_EQUAL(expectedUpdates, updates);
			CPPUNIT_ASSERT_EQUAL(expectedUpdates, postUpdates);
			CPPUNIT_ASSERT(removes.empty());
			CPPUNIT_ASSERT(inserts.empty());
		}

		void testComputeIndexDiff_CommonPrefixAndSuffix() {
			std::vector<char> x = boost::assign::list_of('x')('x')('x')('x')('a')('b')('c')('d')('e')('y')('y')('y');
			std::vector<char> y = boost::assign::list_of('x')('x')('x')('x')('e')('a')('b')('f')('d')('g')('y')('y')('y');

			std::vector<size_t> updates;
			std::vector<size_t> postUpdates;
			std::vector<size_t> removes;
			std::vector<size_t> inserts;
			computeIndexDiff<char, std::equal_to<char>, IsXOrY >(x, y, updates, postUpdates, removes, inserts);
			
			std::vector<size_t> expectedUpdates = boost::assign::list_of(0)(1)(2)(3)(11)(10)(9);
			std::vector<size_t> expectedPostUpdates = boost::assign::list_of(0)(1)(2)(3)(12)(11)(10);
			std::vector<size_t> expectedRemoves = boost::assign::list_of(8)(6);
			std::vector<size_t> expectedInserts = boost::assign::list_of(9)(7)(4);
			CPPUNIT_ASSERT_EQUAL(expectedUpdates, updates);
			CPPUNIT_ASSERT_EQUAL(expectedPostUpdates, postUpdates);
			CPPUNIT_ASSERT_EQUAL(expectedRemoves, removes);
			CPPUNIT_ASSERT_EQUAL(expectedInserts, inserts);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LeastCommonSubsequenceTest);
