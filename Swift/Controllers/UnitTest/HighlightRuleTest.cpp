/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>
#include <vector>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swift/Controllers/HighlightRule.h>

using namespace Swift;

class HighlightRuleTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(HighlightRuleTest);
		CPPUNIT_TEST(testEmptyRuleNeverMatches);
		CPPUNIT_TEST(testKeyword);
		CPPUNIT_TEST(testNickKeyword);
		CPPUNIT_TEST(testNickWithoutOtherKeywords);
		CPPUNIT_TEST(testSender);
		CPPUNIT_TEST(testSenderAndKeyword);
		CPPUNIT_TEST(testWholeWords);
		CPPUNIT_TEST(testCase);
		CPPUNIT_TEST(testWholeWordsAndCase);
		CPPUNIT_TEST(testMUC);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			std::vector<std::string> keywords;
			keywords.push_back("keyword1");
			keywords.push_back("KEYWORD2");

			std::vector<std::string> senders;
			senders.push_back("sender1");
			senders.push_back("SENDER2");

			emptyRule = new HighlightRule();

			keywordRule = new HighlightRule();
			keywordRule->setKeywords(keywords);

			keywordChatRule = new HighlightRule();
			keywordChatRule->setKeywords(keywords);
			keywordChatRule->setMatchChat(true);

			keywordNickChatRule = new HighlightRule();
			keywordNickChatRule->setKeywords(keywords);
			keywordNickChatRule->setNickIsKeyword(true);
			keywordNickChatRule->setMatchChat(true);

			nickChatRule = new HighlightRule();
			nickChatRule->setNickIsKeyword(true);
			nickChatRule->setMatchChat(true);

			nickRule = new HighlightRule();
			nickRule->setNickIsKeyword(true);

			senderRule = new HighlightRule();
			senderRule->setSenders(senders);

			senderChatRule = new HighlightRule();
			senderChatRule->setSenders(senders);
			senderChatRule->setMatchChat(true);

			senderKeywordChatRule = new HighlightRule();
			senderKeywordChatRule->setSenders(senders);
			senderKeywordChatRule->setKeywords(keywords);
			senderKeywordChatRule->setMatchChat(true);

			senderKeywordNickChatRule = new HighlightRule();
			senderKeywordNickChatRule->setSenders(senders);
			senderKeywordNickChatRule->setKeywords(keywords);
			senderKeywordNickChatRule->setNickIsKeyword(true);
			senderKeywordNickChatRule->setMatchChat(true);

			senderKeywordNickWordChatRule = new HighlightRule();
			senderKeywordNickWordChatRule->setSenders(senders);
			senderKeywordNickWordChatRule->setKeywords(keywords);
			senderKeywordNickWordChatRule->setNickIsKeyword(true);
			senderKeywordNickWordChatRule->setMatchWholeWords(true);
			senderKeywordNickWordChatRule->setMatchChat(true);

			senderKeywordNickCaseChatRule = new HighlightRule();
			senderKeywordNickCaseChatRule->setSenders(senders);
			senderKeywordNickCaseChatRule->setKeywords(keywords);
			senderKeywordNickCaseChatRule->setNickIsKeyword(true);
			senderKeywordNickCaseChatRule->setMatchCase(true);
			senderKeywordNickCaseChatRule->setMatchChat(true);

			senderKeywordNickCaseWordChatRule = new HighlightRule();
			senderKeywordNickCaseWordChatRule->setSenders(senders);
			senderKeywordNickCaseWordChatRule->setKeywords(keywords);
			senderKeywordNickCaseWordChatRule->setNickIsKeyword(true);
			senderKeywordNickCaseWordChatRule->setMatchCase(true);
			senderKeywordNickCaseWordChatRule->setMatchWholeWords(true);
			senderKeywordNickCaseWordChatRule->setMatchChat(true);

			senderKeywordNickMUCRule = new HighlightRule();
			senderKeywordNickMUCRule->setSenders(senders);
			senderKeywordNickMUCRule->setKeywords(keywords);
			senderKeywordNickMUCRule->setNickIsKeyword(true);
			senderKeywordNickMUCRule->setMatchMUC(true);
		}

		void tearDown() {
			delete emptyRule;

			delete keywordRule;
			delete keywordChatRule;
			delete keywordNickChatRule;
			delete nickChatRule;
			delete nickRule;

			delete senderRule;
			delete senderChatRule;
			delete senderKeywordChatRule;
			delete senderKeywordNickChatRule;

			delete senderKeywordNickWordChatRule;
			delete senderKeywordNickCaseChatRule;
			delete senderKeywordNickCaseWordChatRule;

			delete senderKeywordNickMUCRule;
		}

		void testEmptyRuleNeverMatches() {
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("body", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("body", "from", "nick", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("body", "from", "", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("body", "from", "", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("body", "", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("body", "", "nick", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("", "from", "nick", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("body", "", "", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("body", "", "", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("", "from", "", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("", "from", "", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("", "", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("", "", "nick", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("", "", "", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(emptyRule->isMatch("", "", "", HighlightRule::MUCMessage), false);
		}

		void testKeyword() {
			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("body", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("body", "from", "nick", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("keyword1", "from", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("keyword1", "from", "nick", HighlightRule::MUCMessage), false);
			CPPUNIT_ASSERT_EQUAL(keywordRule->isMatch("keyword1", "from", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("body", "sender contains keyword1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("abc keyword1 xyz", "from", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("abckeyword1xyz", "from", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("KEYword1", "from", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("abc KEYword1 xyz", "from", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("abcKEYword1xyz", "from", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("keyword2", "from", "nick", HighlightRule::ChatMessage), true);
		}

		void testNickKeyword() {
			CPPUNIT_ASSERT_EQUAL(keywordNickChatRule->isMatch("body contains nick", "from", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(keywordNickChatRule->isMatch("body contains nick", "from", "nick", HighlightRule::MUCMessage), false);
			CPPUNIT_ASSERT_EQUAL(keywordChatRule->isMatch("body contains nick", "from", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(keywordNickChatRule->isMatch("keyword1", "from", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(keywordNickChatRule->isMatch("body", "sender contains nick", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(keywordNickChatRule->isMatch("body contains mixed-case NiCk", "sender", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(keywordNickChatRule->isMatch("nickname", "from", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(keywordNickChatRule->isMatch("NIckNAME", "from", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(keywordNickChatRule->isMatch("body", "from", "", HighlightRule::ChatMessage), false);
		}

		void testNickWithoutOtherKeywords() {
			CPPUNIT_ASSERT_EQUAL(nickChatRule->isMatch("body contains nick", "from", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(nickChatRule->isMatch("body contains nick", "from", "nick", HighlightRule::MUCMessage), false);
			CPPUNIT_ASSERT_EQUAL(nickRule->isMatch("body contains nick", "from", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(nickChatRule->isMatch("keyword1", "from", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(nickChatRule->isMatch("body", "sender contains nick but it does't matter", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(nickChatRule->isMatch("body contains mixed-case NiCk", "from", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(nickChatRule->isMatch("nickname", "from", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(nickChatRule->isMatch("NIckNAME", "from", "nick", HighlightRule::ChatMessage), true);

			// there are no keywords in this rule and empty nick is not treated as a keyword, so we don't check for keywords to get a match
			CPPUNIT_ASSERT_EQUAL(nickChatRule->isMatch("body", "from", "", HighlightRule::ChatMessage), true);
		}

		void testSender() {
			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "from", "nick", HighlightRule::MUCMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "sender1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "sender1", "nick", HighlightRule::MUCMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderRule->isMatch("body", "sender1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderRule->isMatch("body contains sender1", "from", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "abc sender1 xyz", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "abcsender1xyz", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "SENDer1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "abc SENDer1 xyz", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "abcSENDer1xyz", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(senderChatRule->isMatch("body", "sender2", "nick", HighlightRule::ChatMessage), true);
		}

		void testSenderAndKeyword() {
			CPPUNIT_ASSERT_EQUAL(senderKeywordChatRule->isMatch("body", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordChatRule->isMatch("keyword1", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordChatRule->isMatch("body", "sender1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordChatRule->isMatch("keyword1", "sender1", "nick", HighlightRule::ChatMessage), true);
		}

		void testWholeWords() {
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("body", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("keyword1", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("body", "sender1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("keyword1", "sender1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("xkeyword1", "sender1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("keyword1", "xsender1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("body contains nick", "sender1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("body contains xnick", "sender1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("KEYword1", "SENDer1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickWordChatRule->isMatch("body contains NiCk", "sender1", "nick", HighlightRule::ChatMessage), true);
		}

		void testCase() {
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("body", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("keyword1", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("body", "sender1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("keyword1", "sender1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("xkeyword1", "xsender1", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("body contains nick", "sender1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("body contains xnick", "sender1", "nick", HighlightRule::ChatMessage), true);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("KEYword1", "SENDer1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("keyword1", "SENDer1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("KEYword1", "sender1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseChatRule->isMatch("body contains NiCk", "sender1", "nick", HighlightRule::ChatMessage), false);
		}

		void testWholeWordsAndCase() {
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("body", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("keyword1", "from", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("body", "sender1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("keyword1", "sender1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("xkeyword1", "sender1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("keyword1", "xsender1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("body contains nick", "sender1", "nick", HighlightRule::ChatMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("body contains xnick", "sender1", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("KEYword1", "SENDer1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("keyword1", "SENDer1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("KEYword1", "sender1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickCaseWordChatRule->isMatch("body contains NiCk", "sender1", "nick", HighlightRule::ChatMessage), false);
		}

		void testMUC() {
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickMUCRule->isMatch("body", "from", "nick", HighlightRule::ChatMessage), false);

			CPPUNIT_ASSERT_EQUAL(senderKeywordNickMUCRule->isMatch("keyword1", "sender1", "nick", HighlightRule::ChatMessage), false);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickMUCRule->isMatch("keyword1", "sender1", "nick", HighlightRule::MUCMessage), true);
			CPPUNIT_ASSERT_EQUAL(senderKeywordNickMUCRule->isMatch("body contains nick", "sender1", "nick", HighlightRule::MUCMessage), true);
		}

	private:
		HighlightRule* emptyRule;

		HighlightRule* keywordRule;
		HighlightRule* keywordChatRule;
		HighlightRule* keywordNickChatRule;
		HighlightRule* nickChatRule;
		HighlightRule* nickRule;

		HighlightRule* senderRule;
		HighlightRule* senderChatRule;
		HighlightRule* senderKeywordChatRule;
		HighlightRule* senderKeywordNickChatRule;

		HighlightRule* senderKeywordNickWordChatRule;
		HighlightRule* senderKeywordNickCaseChatRule;
		HighlightRule* senderKeywordNickCaseWordChatRule;

		HighlightRule* senderKeywordNickMUCRule;
};

CPPUNIT_TEST_SUITE_REGISTRATION(HighlightRuleTest);
