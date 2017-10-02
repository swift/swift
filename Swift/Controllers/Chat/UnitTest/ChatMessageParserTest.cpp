/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <gtest/gtest.h>

#include <Swift/Controllers/Chat/ChatMessageParser.h>
#include <Swift/Controllers/Highlighting/HighlightConfiguration.h>
#include <Swift/Controllers/Highlighting/HighlightManager.h>
#include <Swift/Controllers/Settings/DummySettingsProvider.h>

using namespace Swift;

// Common test state
class ChatMessageParserTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        smile1_ = ":)";
        smile1Path_ = "/blah/smile1.png";
        smile2_ = ":(";
        smile2Path_ = "/blah/smile2.jpg";
        emoticons_[smile1_] = smile1Path_;
        emoticons_[smile2_] = smile2Path_;
    }

    virtual void TearDown() {
        emoticons_.clear();
    }

    static void assertText(const ChatWindow::ChatMessage& result, size_t index, const std::string& text) {
        ASSERT_LT(index, result.getParts().size());
        std::shared_ptr<ChatWindow::ChatTextMessagePart> part = std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(result.getParts()[index]);
        ASSERT_EQ(text, part->text);
    }

    static void assertEmoticon(const ChatWindow::ChatMessage& result, size_t index, const std::string& text, const std::string& path) {
        ASSERT_LT(index, result.getParts().size());
        std::shared_ptr<ChatWindow::ChatEmoticonMessagePart> part = std::dynamic_pointer_cast<ChatWindow::ChatEmoticonMessagePart>(result.getParts()[index]);
        ASSERT_NE(nullptr, part);
        ASSERT_EQ(text, part->alternativeText);
        ASSERT_EQ(path, part->imagePath);
    }

    static void assertURL(const ChatWindow::ChatMessage& result, size_t index, const std::string& text) {
        ASSERT_LT(index, result.getParts().size());
        std::shared_ptr<ChatWindow::ChatURIMessagePart> part = std::dynamic_pointer_cast<ChatWindow::ChatURIMessagePart>(result.getParts()[index]);
        ASSERT_EQ(text, part->target);
    }

    void assertHighlight(const ChatWindow::ChatMessage& result, size_t index, const std::string& text, const HighlightAction& action) {
        ASSERT_LT(index, result.getParts().size());
        std::shared_ptr<ChatWindow::ChatHighlightingMessagePart> part = std::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(result.getParts()[index]);
        ASSERT_EQ(std::string(text), part->text);
        ASSERT_EQ(action, part->action);
    }

    static const std::shared_ptr<HighlightConfiguration> highlightConfigFromKeyword(const std::string& keyword, bool matchCase) {
        std::shared_ptr<HighlightConfiguration> config = std::make_shared<HighlightConfiguration>();
        HighlightConfiguration::KeywordHightlight keywordHighlight;
        keywordHighlight.keyword = keyword;
        keywordHighlight.matchCaseSensitive = matchCase;
        keywordHighlight.action.setFrontColor(std::string("#121212"));
        config->keywordHighlights.push_back(keywordHighlight);
        return config;
    }

    static const std::shared_ptr<HighlightConfiguration> mergeHighlightConfig(const std::shared_ptr<HighlightConfiguration>& configA, const std::shared_ptr<HighlightConfiguration>& configB) {
        std::shared_ptr<HighlightConfiguration> config = std::make_shared<HighlightConfiguration>();
        config->keywordHighlights.insert(config->keywordHighlights.end(), configA->keywordHighlights.begin(), configA->keywordHighlights.end());
        config->keywordHighlights.insert(config->keywordHighlights.end(), configB->keywordHighlights.begin(), configB->keywordHighlights.end());
        return config;
    }

    std::map<std::string, std::string> emoticons_;
    std::string smile1_;
    std::string smile1Path_;
    std::string smile2_;
    std::string smile2Path_;
};

TEST_F(ChatMessageParserTest, testNoHighlightingWithEmtpyConfiguration) {
    const std::string no_special_message = "a message with no special content";
    ChatMessageParser testling(emoticons_, std::make_shared<HighlightConfiguration>());
    auto result = testling.parseMessageBody(no_special_message);
    assertText(result, 0, no_special_message);
}

TEST_F(ChatMessageParserTest, testSimpleHighlightAndEmojiAndUrlParsing) {
    auto highlightConfig = highlightConfigFromKeyword("trigger", false);
    auto testling = ChatMessageParser(emoticons_, highlightConfig);
    auto result = testling.parseMessageBody(":) shiny :( trigger :) http://wonderland.lit/blah http://denmark.lit boom boom");
    assertEmoticon(result, 0, smile1_, smile1Path_);
    assertText(result, 1, " shiny ");
    assertEmoticon(result, 2, smile2_, smile2Path_);
    assertText(result, 3, " ");
    assertHighlight(result, 4, "trigger", highlightConfig->keywordHighlights[0].action);
    assertText(result, 5, " ");
    assertEmoticon(result, 6, smile1_, smile1Path_);
    assertText(result, 7, " ");
    assertURL(result, 8, "http://wonderland.lit/blah");
    assertText(result, 9, " ");
    assertURL(result, 10, "http://denmark.lit");
    assertText(result, 11, " boom boom");
}

TEST_F(ChatMessageParserTest, testNoKeywordHighlightAsPartOfLongerWords) {
    auto testling = ChatMessageParser(emoticons_, highlightConfigFromKeyword("trigger", false));
    auto result = testling.parseMessageBody("testtriggermessage");
    assertText(result, 0, "testtriggermessage");
}

TEST_F(ChatMessageParserTest, testCaseInsensitiveKeyordHighlight) {
    auto config = highlightConfigFromKeyword("trigger", true);
    auto testling = ChatMessageParser(emoticons_, config);
    auto result = testling.parseMessageBody("TrIgGeR");
    assertText(result, 0, "TrIgGeR");

    testling = ChatMessageParser(emoticons_, highlightConfigFromKeyword("trigger", false));
    result = testling.parseMessageBody("TrIgGeR");
    assertHighlight(result, 0, "TrIgGeR", config->keywordHighlights[0].action);
}

TEST_F(ChatMessageParserTest, testMultipleKeywordHighlights) {
    auto config = mergeHighlightConfig(highlightConfigFromKeyword("one", false), highlightConfigFromKeyword("three", false));
    auto testling = ChatMessageParser(emoticons_, config);
    auto result = testling.parseMessageBody("zero one two three");
    assertText(result, 0, "zero ");
    assertHighlight(result, 1, "one", config->keywordHighlights[0].action);
    assertText(result, 2, " two ");
    assertHighlight(result, 3, "three", config->keywordHighlights[0].action);
}

TEST_F(ChatMessageParserTest, testMultipleCaseInsensitiveKeywordHighlights) {
    auto config = mergeHighlightConfig(highlightConfigFromKeyword("one", false), highlightConfigFromKeyword("three", false));
    auto testling = ChatMessageParser(emoticons_, config);
    auto result = testling.parseMessageBody("zero oNe two tHrEe");
    assertText(result, 0, "zero ");
    assertHighlight(result, 1, "oNe", config->keywordHighlights[0].action);
    assertText(result, 2, " two ");
    assertHighlight(result, 3, "tHrEe", config->keywordHighlights[0].action);
}

TEST_F(ChatMessageParserTest, testMultipleCaseSensitiveKeywordHighlights) {
    auto config = mergeHighlightConfig(highlightConfigFromKeyword("one", false), highlightConfigFromKeyword("three", true));
    auto testling = ChatMessageParser(emoticons_, config);
    auto result = testling.parseMessageBody("zero oNe two tHrEe");
    assertText(result, 0, "zero ");
    assertHighlight(result, 1, "oNe", config->keywordHighlights[0].action);
    assertText(result, 2, " two tHrEe");

    config = mergeHighlightConfig(highlightConfigFromKeyword("one", true), highlightConfigFromKeyword("three", false));
    testling = ChatMessageParser(emoticons_, config);
    result = testling.parseMessageBody("zero oNe two tHrEe");
    assertText(result, 0, "zero oNe two ");
    assertHighlight(result, 1, "tHrEe", config->keywordHighlights[0].action);
}

TEST_F(ChatMessageParserTest, testOneEmoticon) {
    auto testling = ChatMessageParser(emoticons_, std::make_shared<HighlightConfiguration>());
    auto result = testling.parseMessageBody(" :) ");
    assertText(result, 0, " ");
    assertEmoticon(result, 1, smile1_, smile1Path_);
    assertText(result, 2, " ");
}

TEST_F(ChatMessageParserTest, testBareEmoticon) {
    auto testling = ChatMessageParser(emoticons_, std::make_shared<HighlightConfiguration>());
    auto result = testling.parseMessageBody(":)");
    assertEmoticon(result, 0, smile1_, smile1Path_);
}

TEST_F(ChatMessageParserTest, testHiddenEmoticon) {
    auto testling = ChatMessageParser(emoticons_, std::make_shared<HighlightConfiguration>());
    auto result = testling.parseMessageBody("b:)a");
    assertText(result, 0, "b:)a");
}

TEST_F(ChatMessageParserTest, testEndlineEmoticon) {
    auto testling = ChatMessageParser(emoticons_, std::make_shared<HighlightConfiguration>());
    auto result = testling.parseMessageBody("Lazy:)");
    assertText(result, 0, "Lazy");
    assertEmoticon(result, 1, smile1_, smile1Path_);
}

TEST_F(ChatMessageParserTest, testBoundedEmoticons) {
    auto testling = ChatMessageParser(emoticons_, std::make_shared<HighlightConfiguration>());
    auto result = testling.parseMessageBody(":)Lazy:(");
    assertEmoticon(result, 0, smile1_, smile1Path_);
    assertText(result, 1, "Lazy");
    assertEmoticon(result, 2, smile2_, smile2Path_);
}

TEST_F(ChatMessageParserTest, testEmoticonParenthesis) {
    auto testling = ChatMessageParser(emoticons_, std::make_shared<HighlightConfiguration>());
    auto result = testling.parseMessageBody("(Like this :))");
    assertText(result, 0, "(Like this ");
    assertEmoticon(result, 1, smile1_, smile1Path_);
    assertText(result, 2, ")");
}

TEST_F(ChatMessageParserTest, testSenderAndKeywordHighlighting) {
    auto config = std::make_shared<HighlightConfiguration>();
    auto contactHighlight = HighlightConfiguration::ContactHighlight();
    contactHighlight.action.setFrontColor(std::string("#f0f0f0"));
    contactHighlight.action.setBackColor(std::string("#0f0f0f"));
    contactHighlight.name = "Romeo";
    config->contactHighlights.push_back(contactHighlight);
    auto keywordHighlight = HighlightConfiguration::KeywordHightlight();
    keywordHighlight.action.setFrontColor(std::string("#abcdef"));
    keywordHighlight.action.setBackColor(std::string("#fedcba"));
    keywordHighlight.keyword = "XMPP";
    config->keywordHighlights.push_back(keywordHighlight);
    auto testling = ChatMessageParser(emoticons_, config);
    auto result = testling.parseMessageBody("Heard any news about xmpp recently?", "Romeo");
    assertText(result, 0, "Heard any news about ");
    assertHighlight(result, 1, "xmpp", keywordHighlight.action);
    assertText(result, 2, " recently?");
    ASSERT_EQ(contactHighlight.action, result.getHighlightActionSender());
    ASSERT_EQ(HighlightAction(), result.getHighlightActionOwnMention());
}

TEST_F(ChatMessageParserTest, testKeywordWithEmptyActionIsIgnored) {
    auto config = std::make_shared<HighlightConfiguration>();
    auto keywordHighlight = HighlightConfiguration::KeywordHightlight();
    keywordHighlight.keyword = "XMPP";
    config->keywordHighlights.push_back(keywordHighlight);
    auto testling = ChatMessageParser(emoticons_, config);
    auto result = testling.parseMessageBody("Heard any news about xmpp recently?", "Romeo");
    assertText(result, 0, "Heard any news about xmpp recently?");
    ASSERT_EQ(HighlightAction(), result.getHighlightActionOwnMention());
}

TEST_F(ChatMessageParserTest, testMeMessageAndOwnMention) {
    auto config = std::make_shared<HighlightConfiguration>();
    config->ownMentionAction.setFrontColor(std::string("#f0f0f0"));
    config->ownMentionAction.setBackColor(std::string("#0f0f0f"));
    config->ownMentionAction.setSoundFilePath(std::string("someSoundFile.wav"));
    auto ownMentionActionForPart = config->ownMentionAction;
    ownMentionActionForPart.setSoundFilePath(boost::optional<std::string>());
    auto testling = ChatMessageParser(emoticons_, config);
    testling.setNick("Juliet");
    auto result = testling.parseMessageBody("/me wonders when Juliet is coming?", "Romeo");
    assertText(result, 0, "wonders when ");
    assertHighlight(result, 1, "Juliet", ownMentionActionForPart);
    assertText(result, 2, " is coming?");
    ASSERT_EQ(true, result.isMeCommand());
    ASSERT_EQ(config->ownMentionAction, result.getHighlightActionOwnMention());
}

TEST_F(ChatMessageParserTest, testSoundAndNotificationOnDirectMessage) {
    auto defaultConfiguration = std::make_shared<HighlightConfiguration>();
    defaultConfiguration->playSoundOnIncomingDirectMessages = true;
    defaultConfiguration->showNotificationOnIncomingDirectMessages = true;
    defaultConfiguration->ownMentionAction.setFrontColor(std::string("black"));
    defaultConfiguration->ownMentionAction.setBackColor(std::string("yellow"));
    defaultConfiguration->ownMentionAction.setSoundFilePath(std::string(""));
    defaultConfiguration->ownMentionAction.setSystemNotificationEnabled(true);

    auto testling = ChatMessageParser(emoticons_, defaultConfiguration, ChatMessageParser::Mode::Chat);
    auto result = testling.parseMessageBody("I wonder when Juliet is coming?", "Romeo");

    ASSERT_EQ(std::string(""), result.getHighlightActionDirectMessage().getSoundFilePath().get_value_or(std::string("somethingElse")));
    ASSERT_EQ(true, result.getHighlightActionDirectMessage().isSystemNotificationEnabled());
    ASSERT_EQ(HighlightAction(), result.getHighlightActionOwnMention());
}

TEST_F(ChatMessageParserTest, testWithDefaultConfiguration) {
    DummySettingsProvider settings;
    HighlightManager manager(&settings);
    manager.resetToDefaultConfiguration();
    auto testling = ChatMessageParser(emoticons_, manager.getConfiguration(), ChatMessageParser::Mode::GroupChat);
    testling.setNick("Juliet");
    auto result = testling.parseMessageBody("Hello, how is it going?", "Romeo");
    assertText(result, 0, "Hello, how is it going?");
    ASSERT_EQ(HighlightAction(), result.getHighlightActionOwnMention());
    ASSERT_EQ(HighlightAction(), result.getHighlightActionDirectMessage());
    ASSERT_EQ(HighlightAction(), result.getHighlightActionGroupMessage());
    ASSERT_EQ(HighlightAction(), result.getHighlightActionSender());

    result = testling.parseMessageBody("Juliet, seen the new interface design?", "Romeo");
    auto mentionKeywordAction = manager.getConfiguration()->ownMentionAction;
    mentionKeywordAction.setSoundFilePath(boost::optional<std::string>());
    mentionKeywordAction.setSystemNotificationEnabled(false);
    assertHighlight(result, 0, "Juliet", mentionKeywordAction);
    assertText(result, 1, ", seen the new interface design?");
    ASSERT_EQ(manager.getConfiguration()->ownMentionAction, result.getHighlightActionOwnMention());
    ASSERT_EQ(HighlightAction(), result.getHighlightActionDirectMessage());
    ASSERT_EQ(HighlightAction(), result.getHighlightActionGroupMessage());
    ASSERT_EQ(HighlightAction(), result.getHighlightActionSender());
}
