/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <hippomocks.h>

#include <Swift/Controllers/Chat/ChatMessageParser.h>

using namespace Swift;

class ChatMessageParserTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ChatMessageParserTest);
	CPPUNIT_TEST(testFullBody);
	CPPUNIT_TEST(testOneEmoticon);
	CPPUNIT_TEST(testBareEmoticon);
	CPPUNIT_TEST(testHiddenEmoticon);
	CPPUNIT_TEST(testEndlineEmoticon);
	CPPUNIT_TEST(testBoundedEmoticons);
	CPPUNIT_TEST(testNoColourNoHighlight);
	CPPUNIT_TEST_SUITE_END();
	
public:
	void setUp() {
		smile1_ = ":)";
		smile1Path_ = "/blah/smile1.png";
		smile2_ = ":(";
		smile2Path_ = "/blah/smile2.jpg";
		emoticons_[smile1_] = smile1Path_;
		emoticons_[smile2_] = smile2Path_;
	}
	
	void tearDown() {
		emoticons_.clear();
	}

	void assertText(const ChatWindow::ChatMessage& result, size_t index, const std::string& text) {
		boost::shared_ptr<ChatWindow::ChatTextMessagePart> part = boost::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(result.getParts()[index]);
		CPPUNIT_ASSERT_EQUAL(text, part->text);
	}

	void assertEmoticon(const ChatWindow::ChatMessage& result, size_t index, const std::string& text, const std::string& path) {
		boost::shared_ptr<ChatWindow::ChatEmoticonMessagePart> part = boost::dynamic_pointer_cast<ChatWindow::ChatEmoticonMessagePart>(result.getParts()[index]);
		CPPUNIT_ASSERT(!!part);
		CPPUNIT_ASSERT_EQUAL(text, part->alternativeText);
		CPPUNIT_ASSERT_EQUAL(path, part->imagePath);
	}

#define assertHighlight(RESULT, INDEX, TEXT, EXPECTED_HIGHLIGHT) \
	{ \
		boost::shared_ptr<ChatWindow::ChatHighlightingMessagePart> part = boost::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(RESULT.getParts()[INDEX]); \
		CPPUNIT_ASSERT_EQUAL(std::string(TEXT), part->text); \
		CPPUNIT_ASSERT(EXPECTED_HIGHLIGHT == part->action); \
	}

	void assertURL(const ChatWindow::ChatMessage& result, size_t index, const std::string& text) {
		boost::shared_ptr<ChatWindow::ChatURIMessagePart> part = boost::dynamic_pointer_cast<ChatWindow::ChatURIMessagePart>(result.getParts()[index]);
		CPPUNIT_ASSERT_EQUAL(text, part->target);
	}

	static HighlightRule ruleFromKeyword(const std::string& keyword, bool matchCase, bool matchWholeWord)
	{
		HighlightRule rule;
		std::vector<std::string> keywords;
		keywords.push_back(keyword);
		rule.setKeywords(keywords);
		rule.setMatchCase(matchCase);
		rule.setMatchWholeWords(matchWholeWord);
		rule.setMatchChat(true);
		rule.getAction().setTextBackground("white");
		return rule;
	}

	static const HighlightRulesListPtr ruleListFromKeyword(const std::string& keyword, bool matchCase, bool matchWholeWord)
	{
		boost::shared_ptr<HighlightManager::HighlightRulesList> list = boost::make_shared<HighlightManager::HighlightRulesList>();
		list->addRule(ruleFromKeyword(keyword, matchCase, matchWholeWord));
		return list;
	}

	static const HighlightRulesListPtr ruleListFromKeywords(const HighlightRule &rule1, const HighlightRule &rule2)
	{
		boost::shared_ptr<HighlightManager::HighlightRulesList> list = boost::make_shared<HighlightManager::HighlightRulesList>();
		list->addRule(rule1);
		list->addRule(rule2);
		return list;
	}

	static HighlightRulesListPtr ruleListWithNickHighlight(bool withHighlightColour = true)
	{
		HighlightRule rule;
		rule.setMatchChat(true);
		rule.setNickIsKeyword(true);
		rule.setMatchCase(true);
		rule.setMatchWholeWords(true);
		if (withHighlightColour) {
			rule.getAction().setTextBackground("white");
		}
		boost::shared_ptr<HighlightManager::HighlightRulesList> list = boost::make_shared<HighlightManager::HighlightRulesList>();
		list->addRule(rule);
		return list;
	}

	void testFullBody() {
		const std::string no_special_message = "a message with no special content";
		ChatMessageParser testling(emoticons_, boost::make_shared<HighlightManager::HighlightRulesList>());
		ChatWindow::ChatMessage result = testling.parseMessageBody(no_special_message);
		assertText(result, 0, no_special_message);

		HighlightRulesListPtr highlightRuleList = ruleListFromKeyword("trigger", false, false);
		testling = ChatMessageParser(emoticons_, highlightRuleList);
		result = testling.parseMessageBody(":) shiny :( trigger :) http://wonderland.lit/blah http://denmark.lit boom boom");
		assertEmoticon(result, 0, smile1_, smile1Path_);
		assertText(result, 1, " shiny ");
		assertEmoticon(result, 2, smile2_, smile2Path_);
		assertText(result, 3, " ");
		assertHighlight(result, 4, "trigger", highlightRuleList->getRule(0).getAction());
		assertText(result, 5, " ");
		assertEmoticon(result, 6, smile1_, smile1Path_);
		assertText(result, 7, " ");
		assertURL(result, 8, "http://wonderland.lit/blah");
		assertText(result, 9, " ");
		assertURL(result, 10, "http://denmark.lit");
		assertText(result, 11, " boom boom");

		testling = ChatMessageParser(emoticons_, ruleListFromKeyword("trigger", false, false));
		result = testling.parseMessageBody("testtriggermessage");
		assertText(result, 0, "test");
		assertHighlight(result, 1, "trigger", highlightRuleList->getRule(0).getAction());
		assertText(result, 2, "message");

		testling = ChatMessageParser(emoticons_, ruleListFromKeyword("trigger", false, true));
		result = testling.parseMessageBody("testtriggermessage");
		assertText(result, 0, "testtriggermessage");

		testling = ChatMessageParser(emoticons_, ruleListFromKeyword("trigger", true, false));
		result = testling.parseMessageBody("TrIgGeR");
		assertText(result, 0, "TrIgGeR");

		testling = ChatMessageParser(emoticons_, ruleListFromKeyword("trigger", false, false));
		result = testling.parseMessageBody("TrIgGeR");
		assertHighlight(result, 0, "TrIgGeR", highlightRuleList->getRule(0).getAction());

		testling = ChatMessageParser(emoticons_, ruleListFromKeyword("trigger", false, false));
		result = testling.parseMessageBody("partialTrIgGeRmatch");
		assertText(result, 0, "partial");
		assertHighlight(result, 1, "TrIgGeR", highlightRuleList->getRule(0).getAction());
		assertText(result, 2, "match");

		testling = ChatMessageParser(emoticons_, ruleListFromKeywords(ruleFromKeyword("one", false, false), ruleFromKeyword("three", false, false)));
		result = testling.parseMessageBody("zero one two three");
		assertText(result, 0, "zero ");
		assertHighlight(result, 1, "one", highlightRuleList->getRule(0).getAction());
		assertText(result, 2, " two ");
		assertHighlight(result, 3, "three", highlightRuleList->getRule(0).getAction());

		testling = ChatMessageParser(emoticons_, ruleListFromKeywords(ruleFromKeyword("one", false, false), ruleFromKeyword("three", false, false)));
		result = testling.parseMessageBody("zero oNe two tHrEe");
		assertText(result, 0, "zero ");
		assertHighlight(result, 1, "oNe", highlightRuleList->getRule(0).getAction());
		assertText(result, 2, " two ");
		assertHighlight(result, 3, "tHrEe", highlightRuleList->getRule(0).getAction());

		testling = ChatMessageParser(emoticons_, ruleListFromKeywords(ruleFromKeyword("one", false, false), ruleFromKeyword("three", true, false)));
		result = testling.parseMessageBody("zero oNe two tHrEe");
		assertText(result, 0, "zero ");
		assertHighlight(result, 1, "oNe", highlightRuleList->getRule(0).getAction());
		assertText(result, 2, " two tHrEe");

		testling = ChatMessageParser(emoticons_, ruleListFromKeywords(ruleFromKeyword("one", true, false), ruleFromKeyword("three", true, false)));
		result = testling.parseMessageBody("zero oNe two tHrEe");
		assertText(result, 0, "zero oNe two tHrEe");

		testling = ChatMessageParser(emoticons_, ruleListFromKeywords(ruleFromKeyword("one", false, false), ruleFromKeyword("three", false, false)));
		result = testling.parseMessageBody("zeroonetwothree");
		assertText(result, 0, "zero");
		assertHighlight(result, 1, "one", highlightRuleList->getRule(0).getAction());
		assertText(result, 2, "two");
		assertHighlight(result, 3, "three", highlightRuleList->getRule(0).getAction());

		testling = ChatMessageParser(emoticons_, ruleListFromKeywords(ruleFromKeyword("one", true, false), ruleFromKeyword("three", false, false)));
		result = testling.parseMessageBody("zeroOnEtwoThReE");
		assertText(result, 0, "zeroOnEtwo");
		assertHighlight(result, 1, "ThReE", highlightRuleList->getRule(0).getAction());

		testling = ChatMessageParser(emoticons_, ruleListFromKeywords(ruleFromKeyword("one", false, true), ruleFromKeyword("three", false, false)));
		result = testling.parseMessageBody("zeroonetwothree");
		assertText(result, 0, "zeroonetwo");
		assertHighlight(result, 1, "three", highlightRuleList->getRule(0).getAction());

		testling = ChatMessageParser(emoticons_, ruleListFromKeywords(ruleFromKeyword("one", false, true), ruleFromKeyword("three", false, true)));
		result = testling.parseMessageBody("zeroonetwothree");
		assertText(result, 0, "zeroonetwothree");

		testling = ChatMessageParser(emoticons_, ruleListWithNickHighlight());
		result = testling.parseMessageBody("Alice", "Alice");
		assertHighlight(result, 0, "Alice", highlightRuleList->getRule(0).getAction());

		testling = ChatMessageParser(emoticons_, ruleListWithNickHighlight());
		result = testling.parseMessageBody("TextAliceText", "Alice");
		assertText(result, 0, "TextAliceText");

		testling = ChatMessageParser(emoticons_, ruleListWithNickHighlight());
		result = testling.parseMessageBody("Text Alice Text", "Alice");
		assertText(result, 0, "Text ");
		assertHighlight(result, 1, "Alice", highlightRuleList->getRule(0).getAction());
		assertText(result, 2, " Text");

		testling = ChatMessageParser(emoticons_, ruleListWithNickHighlight());
		result = testling.parseMessageBody("Alice Text", "Alice");
		assertHighlight(result, 0, "Alice", highlightRuleList->getRule(0).getAction());
		assertText(result, 1, " Text");

		testling = ChatMessageParser(emoticons_, ruleListWithNickHighlight());
		result = testling.parseMessageBody("Text Alice", "Alice");
		assertText(result, 0, "Text ");
		assertHighlight(result, 1, "Alice", highlightRuleList->getRule(0).getAction());
	}

	void testOneEmoticon() {
		ChatMessageParser testling(emoticons_, boost::make_shared<HighlightManager::HighlightRulesList>());
		ChatWindow::ChatMessage result = testling.parseMessageBody(" :) ");
		assertText(result, 0, " ");
		assertEmoticon(result, 1, smile1_, smile1Path_);
		assertText(result, 2, " ");
	}


	void testBareEmoticon() {
		ChatMessageParser testling(emoticons_, boost::make_shared<HighlightManager::HighlightRulesList>());
		ChatWindow::ChatMessage result = testling.parseMessageBody(":)");
		assertEmoticon(result, 0, smile1_, smile1Path_);
	}

	void testHiddenEmoticon() {
		ChatMessageParser testling(emoticons_, boost::make_shared<HighlightManager::HighlightRulesList>());
		ChatWindow::ChatMessage result = testling.parseMessageBody("b:)a");
		assertText(result, 0, "b:)a");
	}

	void testEndlineEmoticon() {
		ChatMessageParser testling(emoticons_, boost::make_shared<HighlightManager::HighlightRulesList>());
		ChatWindow::ChatMessage result = testling.parseMessageBody("Lazy:)");
		assertText(result, 0, "Lazy");
		assertEmoticon(result, 1, smile1_, smile1Path_);
	}

	void testBoundedEmoticons() {
		ChatMessageParser testling(emoticons_, boost::make_shared<HighlightManager::HighlightRulesList>());
		ChatWindow::ChatMessage result = testling.parseMessageBody(":)Lazy:(");
		assertEmoticon(result, 0, smile1_, smile1Path_);
		assertText(result, 1, "Lazy");
		assertEmoticon(result, 2, smile2_, smile2Path_);
	}

	void testEmoticonParenthesis() {
		ChatMessageParser testling(emoticons_, boost::make_shared<HighlightManager::HighlightRulesList>());
		ChatWindow::ChatMessage result = testling.parseMessageBody("(Like this :))");
		assertText(result, 0, "(Like this ");
		assertEmoticon(result, 1, smile1_, smile1Path_);
		assertText(result, 2, ")");
	}

	void testNoColourNoHighlight() {
		ChatMessageParser testling(emoticons_, ruleListWithNickHighlight(false));
		ChatWindow::ChatMessage result = testling.parseMessageBody("Alice", "Alice");
		assertText(result, 0, "Alice");
	}

private:
	std::map<std::string, std::string> emoticons_;
	std::string smile1_;
	std::string smile1Path_;
	std::string smile2_;
	std::string smile2Path_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatMessageParserTest);
