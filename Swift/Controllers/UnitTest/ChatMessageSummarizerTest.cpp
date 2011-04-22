/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swift/Controllers/ChatMessageSummarizer.h"

using namespace Swift;
using namespace std;

class ChatMessageSummarizerTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ChatMessageSummarizerTest);
	CPPUNIT_TEST(testEmpty);
	CPPUNIT_TEST(testCurrentNone);
	CPPUNIT_TEST(testCurrentCount);
	CPPUNIT_TEST(testCurrentCountOthersNone);
	CPPUNIT_TEST(testCurrentCountOtherCount);
	CPPUNIT_TEST(testCurrentNoneOtherCount);
	CPPUNIT_TEST(testCurrentCountOthersCount);
	CPPUNIT_TEST(testCurrentNoneOthersCount);
	CPPUNIT_TEST(testCurrentCountSomeOthersCount);
	CPPUNIT_TEST_SUITE_END();

public:
	ChatMessageSummarizerTest() {};

	void setUp() {

	}

	void testEmpty() {
		string current("");
		vector<UnreadPair> unreads;
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(current, summary.getSummary(current, unreads));
	}

	void testCurrentNone() {
		string current("Bob");
		vector<UnreadPair> unreads;
		unreads.push_back(UnreadPair("Bob", 0));
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(current, summary.getSummary(current, unreads));
	}

	void testCurrentCount() {
		string current("Bob");
		vector<UnreadPair> unreads;
		unreads.push_back(UnreadPair("Bob", 3));
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(string("Bob (3)"), summary.getSummary(current, unreads));
	}

	void testCurrentCountOthersNone() {
		string current("Bob");
		vector<UnreadPair> unreads;
		unreads.push_back(UnreadPair("Bert", 0));
		unreads.push_back(UnreadPair("Bob", 3));
		unreads.push_back(UnreadPair("Betty", 0));
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(string("Bob (3)"), summary.getSummary(current, unreads));
	}

	void testCurrentCountOtherCount() {
		string current("Bob");
		vector<UnreadPair> unreads;
		unreads.push_back(UnreadPair("Bert", 0));
		unreads.push_back(UnreadPair("Bob", 3));
		unreads.push_back(UnreadPair("Betty", 7));
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(string("Bob (3), Betty (7)"), summary.getSummary(current, unreads));
	}

	void testCurrentNoneOtherCount() {
		string current("Bob");
		vector<UnreadPair> unreads;
		unreads.push_back(UnreadPair("Bert", 0));
		unreads.push_back(UnreadPair("Bob", 0));
		unreads.push_back(UnreadPair("Betty", 7));
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(string("Bob, Betty (7)"), summary.getSummary(current, unreads));
	}

	void testCurrentNoneOthersCount() {
		string current("Bob");
		vector<UnreadPair> unreads;
		unreads.push_back(UnreadPair("Bert", 2));
		unreads.push_back(UnreadPair("Bob", 0));
		unreads.push_back(UnreadPair("Betty", 7));
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(string("Bob and 2 others (9)"), summary.getSummary(current, unreads));
	}

	void testCurrentCountOthersCount() {
		string current("Bob");
		vector<UnreadPair> unreads;
		unreads.push_back(UnreadPair("Bert", 2));
		unreads.push_back(UnreadPair("Bob", 11));
		unreads.push_back(UnreadPair("Betty", 7));
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(string("Bob (11) and 2 others (9)"), summary.getSummary(current, unreads));
	}

	void testCurrentCountSomeOthersCount() {
		string current("Bob");
		vector<UnreadPair> unreads;
		unreads.push_back(UnreadPair("Bert", 2));
		unreads.push_back(UnreadPair("Beverly", 0));
		unreads.push_back(UnreadPair("Bob", 11));
		unreads.push_back(UnreadPair("Beatrice", 0));
		unreads.push_back(UnreadPair("Betty", 7));
		ChatMessageSummarizer summary;
		CPPUNIT_ASSERT_EQUAL(string("Bob (11) and 2 others (9)"), summary.getSummary(current, unreads));
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatMessageSummarizerTest);
