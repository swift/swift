/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <hippomocks.h>

#include <Swift/Controllers/Chat/ChatMessageParser.h>

using namespace Swift;

class ChatMessageParserTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ChatMessageParserTest);
	CPPUNIT_TEST(testFullBody);
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
		CPPUNIT_ASSERT_EQUAL(text, part->alternativeText);
		CPPUNIT_ASSERT_EQUAL(path, part->imagePath);
	}

	void assertURL(const ChatWindow::ChatMessage& result, size_t index, const std::string& text) {
		boost::shared_ptr<ChatWindow::ChatURIMessagePart> part = boost::dynamic_pointer_cast<ChatWindow::ChatURIMessagePart>(result.getParts()[index]);
		CPPUNIT_ASSERT_EQUAL(text, part->target);
	}

	void testFullBody() {
		ChatMessageParser testling(emoticons_);
		ChatWindow::ChatMessage result = testling.parseMessageBody(":) shiny :( :) http://wonderland.lit/blah http://denmark.lit boom boom");
		assertEmoticon(result, 0, smile1_, smile1Path_);
		assertText(result, 1, " shiny ");
		assertEmoticon(result, 2, smile2_, smile2Path_);
		assertText(result, 3, " ");
		assertEmoticon(result, 4, smile1_, smile1Path_);
		assertText(result, 5, " ");
		assertURL(result, 6, "http://wonderland.lit/blah");
		assertText(result, 7, " ");
		assertURL(result, 8, "http://denmark.lit");
		assertText(result, 9, " boom boom");
	}

private:
	std::map<std::string, std::string> emoticons_;
	std::string smile1_;
	std::string smile1Path_;
	std::string smile2_;
	std::string smile2Path_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatMessageParserTest);

