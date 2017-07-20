/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/version.hpp>

#include <gtest/gtest.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/StringCodecs/Base64.h>

#include <Swift/Controllers/Chat/ChatListWindowChatBoostSerialize.h>
#include <Swift/Controllers/UIInterfaces/ChatListWindow.h>

using namespace Swift;

class ChatListWindowChatTest : public ::testing::Test {
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}

    void testOptionalPasswordValue(const boost::optional<std::string>& value1, const std::string& value2) {
        auto archiveLibraryVersion = boost::archive::BOOST_ARCHIVE_VERSION();
        if (archiveLibraryVersion != Swift::BoostArchiveSkipVersion) {
            EXPECT_EQ(value1.get_value_or(""), value2);
        }
        else {
            EXPECT_EQ(value1.get_value_or(""), "");
        }
    }

    std::string chatsSerialise(const std::vector<ChatListWindow::Chat>& chats) {
        std::stringstream serializeStream;
        boost::archive::text_oarchive oa(serializeStream);
        oa & chats;
        std::string serializedStr = Base64::encode(createByteArray(serializeStream.str()));
        return serializedStr;
    }

    std::vector<ChatListWindow::Chat> chatsDeserialise(const std::string& b64chats) {
        ByteArray debase64 = Base64::decode(b64chats);
        std::vector<ChatListWindow::Chat> recentChats;
        std::stringstream deserializeStream(std::string(reinterpret_cast<const char*>(vecptr(debase64)), debase64.size()));
        try {
            boost::archive::text_iarchive ia(deserializeStream);
            ia >> recentChats;
        }
        catch (const boost::archive::archive_exception& e) {
            EXPECT_TRUE(false) << "Archive Version:" << boost::archive::BOOST_ARCHIVE_VERSION() << " " << e.what() << std::endl;
            recentChats.clear();
            return recentChats;
        }
        recentChats.erase(std::remove(recentChats.begin(), recentChats.end(), ChatListWindow::Chat()), recentChats.end());
        return recentChats;
    }
};

TEST_F(ChatListWindowChatTest, testNormalSerialization) {
    ChatListWindow::Chat chat1("swift@rooms.swift.im", "swift@rooms.swift.im", "Some text 0", 0, StatusShow::None, "", false, false, "Nick Name");
    ChatListWindow::Chat chat2("testuser1@domain.com", "swift@rooms2.swift.im", "Some text 1", 0, StatusShow::None, "", false, false, "Nick Name", std::string("pass"));
    ChatListWindow::Chat chat3("testuser2@domain.com", "room 2", "Some text 2", 0, StatusShow::None, "", true, false, "Nick Name");
    ChatListWindow::Chat chat4("testuser3@domain.com", "room 3", "Some text 2", 0, StatusShow::None, "", true, false, "Nick Name", std::string("pass"));

    std::map<std::string, JID> impromptuJIDs;
    impromptuJIDs["testuser1@domain.com"] = "testuser1@domain.com";
    impromptuJIDs["testuser2@domain.com"] = "testuser2@domain.com";
    std::map<JID, std::string> inviteesNames;
    inviteesNames["user1@domain.com"] = "user1@domain.com";

    chat3.impromptuJIDs = impromptuJIDs;
    chat3.inviteesNames = inviteesNames;
    chat4.impromptuJIDs = impromptuJIDs;
    chat4.inviteesNames = inviteesNames;

    std::vector<ChatListWindow::Chat> chats;
    chats.push_back(chat1);
    chats.push_back(chat2);
    chats.push_back(chat3);
    chats.push_back(chat4);

    auto base64 = chatsSerialise(chats);
    ASSERT_TRUE(base64.size() > 0);
    auto restoredChats = chatsDeserialise(base64);
    ASSERT_EQ(restoredChats.size(), 4);

    EXPECT_FALSE(restoredChats[0].isMUC);
    EXPECT_EQ(restoredChats[0].jid, "swift@rooms.swift.im");
    EXPECT_EQ(restoredChats[0].chatName, "swift@rooms.swift.im");
    EXPECT_EQ(restoredChats[0].activity, "Some text 0");
    EXPECT_EQ(restoredChats[0].nick, "Nick Name");
    EXPECT_EQ(restoredChats[0].impromptuJIDs.size(), 0);
    testOptionalPasswordValue(restoredChats[0].password, "");
    EXPECT_EQ(restoredChats[0].inviteesNames.size(), 0);

    EXPECT_FALSE(restoredChats[1].isMUC);
    EXPECT_EQ(restoredChats[1].jid, "testuser1@domain.com");
    EXPECT_EQ(restoredChats[1].chatName, "swift@rooms2.swift.im");
    EXPECT_EQ(restoredChats[1].activity, "Some text 1");
    EXPECT_EQ(restoredChats[1].nick, "Nick Name");
    EXPECT_EQ(restoredChats[1].impromptuJIDs.size(), 0);
    testOptionalPasswordValue(restoredChats[1].password, "pass");
    EXPECT_EQ(restoredChats[1].inviteesNames.size(), 0);

    EXPECT_TRUE(restoredChats[2].isMUC);
    EXPECT_EQ(restoredChats[2].jid, "testuser2@domain.com");
    EXPECT_EQ(restoredChats[2].chatName, "room 2");
    EXPECT_EQ(restoredChats[2].activity, "Some text 2");
    EXPECT_EQ(restoredChats[2].nick, "Nick Name");
    ASSERT_EQ(restoredChats[2].impromptuJIDs.size(), 2);
    EXPECT_EQ(restoredChats[2].impromptuJIDs["testuser1@domain.com"], "testuser1@domain.com");
    EXPECT_EQ(restoredChats[2].impromptuJIDs["testuser2@domain.com"], "testuser2@domain.com");
    testOptionalPasswordValue(restoredChats[2].password, "");
    ASSERT_EQ(restoredChats[2].inviteesNames.size(), 1);
    EXPECT_EQ(restoredChats[2].inviteesNames["user1@domain.com"], "user1@domain.com");

    EXPECT_TRUE(restoredChats[3].isMUC);
    EXPECT_EQ(restoredChats[3].jid, "testuser3@domain.com");
    EXPECT_EQ(restoredChats[3].chatName, "room 3");
    EXPECT_EQ(restoredChats[3].activity, "Some text 2");
    EXPECT_EQ(restoredChats[3].nick, "Nick Name");
    ASSERT_EQ(restoredChats[3].impromptuJIDs.size(), 2);
    EXPECT_EQ(restoredChats[3].impromptuJIDs["testuser1@domain.com"], "testuser1@domain.com");
    EXPECT_EQ(restoredChats[3].impromptuJIDs["testuser2@domain.com"], "testuser2@domain.com");
    testOptionalPasswordValue(restoredChats[3].password, "pass");
    ASSERT_EQ(restoredChats[3].inviteesNames.size(), 1);
    EXPECT_EQ(restoredChats[3].inviteesNames["user1@domain.com"], "user1@domain.com");
}

TEST_F(ChatListWindowChatTest, testVersionsSerialization) {
    auto archiveLibraryVersion = boost::archive::BOOST_ARCHIVE_VERSION();
    /*
    The following strings are base64 serialised vectors with these Swift::ChatListWindow::Chat elements:

    Chat1: Jid = "swift@rooms.swift.im", ChatName = "swift@rooms.swift.im", Activity = "Some text 0", isMUC = false, nick="Nick Name"
    Chat2: Jid = "testuser1@domain.com", ChatName = "swift@rooms2.swift.im", Activity = "Some text 1", isMUC = false, nick="Nick Name", password = "pass"
    Chat3: Jid = "testuser2@domain.com", ChatName = "room2", Activity = "Some text 2", isMUC = true, nick="Nick Name", impromptuJIDs, inviteesNames
    Chat4: Jid = "testuser3@domain.com", ChatName = "room3", Activity = "Some text 2", isMUC = true, nick="Nick Name", impromptuJIDs, password = "pass", inviteesNames

    impromptuJIDs = {("testuser1@domain.com","testuser1@domain.com"), ("testuser2@domain.com", "testuser2@domain.com")}
    inviteesNames = {("user1@domain.com","user1@domain.com")}
    */
    std::string serializedChats_BoostArchiveV10_ClassVersion_0 = "MjIgc2VyaWFsaXphdGlvbjo6YXJjaGl2ZSAxMCAwIDAgNCAwIDAgMCAwIDAgMjAgc3dpZnRAcm9vbXMuc3dpZnQuaW0gMjAgc3dpZnRAcm9vbXMuc3dpZnQuaW0gMTEgU29tZSB0ZXh0IDAgMCA5IE5pY2sgTmFtZSAwIDAgMCAwIDIwIHRlc3R1c2VyMUBkb21haW4uY29tIDIxIHN3aWZ0QHJvb21zMi5zd2lmdC5pbSAxMSBTb21lIHRleHQgMSAwIDkgTmljayBOYW1lIDAgMCAyMCB0ZXN0dXNlcjJAZG9tYWluLmNvbSA2IHJvb20gMiAxMSBTb21lIHRleHQgMiAxIDkgTmljayBOYW1lIDIgMCAwIDAgMjAgdGVzdHVzZXIxQGRvbWFpbi5jb20gMjAgdGVzdHVzZXIxQGRvbWFpbi5jb20gMjAgdGVzdHVzZXIyQGRvbWFpbi5jb20gMjAgdGVzdHVzZXIyQGRvbWFpbi5jb20gMjAgdGVzdHVzZXIzQGRvbWFpbi5jb20gNiByb29tIDMgMTEgU29tZSB0ZXh0IDIgMSA5IE5pY2sgTmFtZSAyIDAgMjAgdGVzdHVzZXIxQGRvbWFpbi5jb20gMjAgdGVzdHVzZXIxQGRvbWFpbi5jb20gMjAgdGVzdHVzZXIyQGRvbWFpbi5jb20gMjAgdGVzdHVzZXIyQGRvbWFpbi5jb20=";
    {
        auto restoredChats = chatsDeserialise(serializedChats_BoostArchiveV10_ClassVersion_0);
        if (archiveLibraryVersion == Swift::BoostArchiveSkipVersion) {
            ASSERT_EQ(restoredChats.size(), 0);
        }
        else {
            ASSERT_EQ(restoredChats.size(), 4);

            EXPECT_FALSE(restoredChats[0].isMUC);
            EXPECT_EQ(restoredChats[0].jid, "swift@rooms.swift.im");
            EXPECT_EQ(restoredChats[0].chatName, "swift@rooms.swift.im");
            EXPECT_EQ(restoredChats[0].activity, "Some text 0");
            EXPECT_EQ(restoredChats[0].nick, "Nick Name");
            EXPECT_EQ(restoredChats[0].impromptuJIDs.size(), 0);

            EXPECT_FALSE(restoredChats[1].isMUC);
            EXPECT_EQ(restoredChats[1].jid, "testuser1@domain.com");
            EXPECT_EQ(restoredChats[1].chatName, "swift@rooms2.swift.im");
            EXPECT_EQ(restoredChats[1].activity, "Some text 1");
            EXPECT_EQ(restoredChats[1].nick, "Nick Name");
            EXPECT_EQ(restoredChats[1].impromptuJIDs.size(), 0);

            EXPECT_TRUE(restoredChats[2].isMUC);
            EXPECT_EQ(restoredChats[2].jid, "testuser2@domain.com");
            EXPECT_EQ(restoredChats[2].chatName, "room 2");
            EXPECT_EQ(restoredChats[2].activity, "Some text 2");
            EXPECT_EQ(restoredChats[2].nick, "Nick Name");
            ASSERT_EQ(restoredChats[2].impromptuJIDs.size(), 2);
            EXPECT_EQ(restoredChats[2].impromptuJIDs["testuser1@domain.com"], "testuser1@domain.com");
            EXPECT_EQ(restoredChats[2].impromptuJIDs["testuser2@domain.com"], "testuser2@domain.com");

            EXPECT_TRUE(restoredChats[3].isMUC);
            EXPECT_EQ(restoredChats[3].jid, "testuser3@domain.com");
            EXPECT_EQ(restoredChats[3].chatName, "room 3");
            EXPECT_EQ(restoredChats[3].activity, "Some text 2");
            EXPECT_EQ(restoredChats[3].nick, "Nick Name");
            ASSERT_EQ(restoredChats[3].impromptuJIDs.size(), 2);
            EXPECT_EQ(restoredChats[3].impromptuJIDs["testuser1@domain.com"], "testuser1@domain.com");
            EXPECT_EQ(restoredChats[3].impromptuJIDs["testuser2@domain.com"], "testuser2@domain.com");
        }
    }

    std::string serializedChats_BoostArchiveV10_ClassVersion_1 = "MjIgc2VyaWFsaXphdGlvbjo6YXJjaGl2ZSAxMCAwIDAgNCAxIDAgMSAwIDAgMjAgc3dpZnRAcm9vbXMuc3dpZnQuaW0gMjAgc3dpZnRAcm9vbXMuc3dpZnQuaW0gMTEgU29tZSB0ZXh0IDAgMCA5IE5pY2sgTmFtZSAwIDAgMCAwIDAgMCAwIDIwIHRlc3R1c2VyMUBkb21haW4uY29tIDIxIHN3aWZ0QHJvb21zMi5zd2lmdC5pbSAxMSBTb21lIHRleHQgMSAwIDkgTmljayBOYW1lIDAgMCAxIDAgNCBwYXNzIDIwIHRlc3R1c2VyMkBkb21haW4uY29tIDYgcm9vbSAyIDExIFNvbWUgdGV4dCAyIDEgOSBOaWNrIE5hbWUgMiAwIDAgMCAyMCB0ZXN0dXNlcjFAZG9tYWluLmNvbSAyMCB0ZXN0dXNlcjFAZG9tYWluLmNvbSAyMCB0ZXN0dXNlcjJAZG9tYWluLmNvbSAyMCB0ZXN0dXNlcjJAZG9tYWluLmNvbSAwIDIwIHRlc3R1c2VyM0Bkb21haW4uY29tIDYgcm9vbSAzIDExIFNvbWUgdGV4dCAyIDEgOSBOaWNrIE5hbWUgMiAwIDIwIHRlc3R1c2VyMUBkb21haW4uY29tIDIwIHRlc3R1c2VyMUBkb21haW4uY29tIDIwIHRlc3R1c2VyMkBkb21haW4uY29tIDIwIHRlc3R1c2VyMkBkb21haW4uY29tIDEgMCA0IHBhc3M=";
    {
        auto restoredChats = chatsDeserialise(serializedChats_BoostArchiveV10_ClassVersion_1);
        if (archiveLibraryVersion == Swift::BoostArchiveSkipVersion) {
            ASSERT_EQ(restoredChats.size(), 0);
        }
        else {
            ASSERT_EQ(restoredChats.size(), 4);

            EXPECT_FALSE(restoredChats[0].isMUC);
            EXPECT_EQ(restoredChats[0].jid, "swift@rooms.swift.im");
            EXPECT_EQ(restoredChats[0].chatName, "swift@rooms.swift.im");
            EXPECT_EQ(restoredChats[0].activity, "Some text 0");
            EXPECT_EQ(restoredChats[0].nick, "Nick Name");
            EXPECT_EQ(restoredChats[0].impromptuJIDs.size(), 0);
            EXPECT_EQ(restoredChats[0].password.get_value_or(""), "");

            EXPECT_FALSE(restoredChats[1].isMUC);
            EXPECT_EQ(restoredChats[1].jid, "testuser1@domain.com");
            EXPECT_EQ(restoredChats[1].chatName, "swift@rooms2.swift.im");
            EXPECT_EQ(restoredChats[1].activity, "Some text 1");
            EXPECT_EQ(restoredChats[1].nick, "Nick Name");
            EXPECT_EQ(restoredChats[1].impromptuJIDs.size(), 0);
            EXPECT_EQ(restoredChats[1].password.get_value_or(""), "pass");

            EXPECT_TRUE(restoredChats[2].isMUC);
            EXPECT_EQ(restoredChats[2].jid, "testuser2@domain.com");
            EXPECT_EQ(restoredChats[2].chatName, "room 2");
            EXPECT_EQ(restoredChats[2].activity, "Some text 2");
            EXPECT_EQ(restoredChats[2].nick, "Nick Name");
            ASSERT_EQ(restoredChats[2].impromptuJIDs.size(), 2);
            EXPECT_EQ(restoredChats[2].impromptuJIDs["testuser1@domain.com"], "testuser1@domain.com");
            EXPECT_EQ(restoredChats[2].impromptuJIDs["testuser2@domain.com"], "testuser2@domain.com");
            EXPECT_EQ(restoredChats[2].password.get_value_or(""), "");

            EXPECT_TRUE(restoredChats[3].isMUC);
            EXPECT_EQ(restoredChats[3].jid, "testuser3@domain.com");
            EXPECT_EQ(restoredChats[3].chatName, "room 3");
            EXPECT_EQ(restoredChats[3].activity, "Some text 2");
            EXPECT_EQ(restoredChats[3].nick, "Nick Name");
            ASSERT_EQ(restoredChats[3].impromptuJIDs.size(), 2);
            EXPECT_EQ(restoredChats[3].impromptuJIDs["testuser1@domain.com"], "testuser1@domain.com");
            EXPECT_EQ(restoredChats[3].impromptuJIDs["testuser2@domain.com"], "testuser2@domain.com");
            EXPECT_EQ(restoredChats[3].password.get_value_or(""), "pass");
        }
    }

    std::string serializedChats_BoostArchiveV10_ClassVersion_2 = "MjIgc2VyaWFsaXphdGlvbjo6YXJjaGl2ZSAxMCAwIDAgNCAyIDAgMiAwIDAgMjAgc3dpZnRAcm9vbXMuc3dpZnQuaW0gMjAgc3dpZnRAcm9vbXMuc3dpZnQuaW0gMTEgU29tZSB0ZXh0IDAgMCA5IE5pY2sgTmFtZSAwIDAgMCAwIDAgMCAwIDAgMCAwIDAgMjAgdGVzdHVzZXIxQGRvbWFpbi5jb20gMjEgc3dpZnRAcm9vbXMyLnN3aWZ0LmltIDExIFNvbWUgdGV4dCAxIDAgOSBOaWNrIE5hbWUgMCAwIDEgMCA0IHBhc3MgMCAwIDIwIHRlc3R1c2VyMkBkb21haW4uY29tIDYgcm9vbSAyIDExIFNvbWUgdGV4dCAyIDEgOSBOaWNrIE5hbWUgMiAwIDAgMCAyMCB0ZXN0dXNlcjFAZG9tYWluLmNvbSAyMCB0ZXN0dXNlcjFAZG9tYWluLmNvbSAyMCB0ZXN0dXNlcjJAZG9tYWluLmNvbSAyMCB0ZXN0dXNlcjJAZG9tYWluLmNvbSAwIDEgMCAwIDAgMTYgdXNlcjFAZG9tYWluLmNvbSAxNiB1c2VyMUBkb21haW4uY29tIDIwIHRlc3R1c2VyM0Bkb21haW4uY29tIDYgcm9vbSAzIDExIFNvbWUgdGV4dCAyIDEgOSBOaWNrIE5hbWUgMiAwIDIwIHRlc3R1c2VyMUBkb21haW4uY29tIDIwIHRlc3R1c2VyMUBkb21haW4uY29tIDIwIHRlc3R1c2VyMkBkb21haW4uY29tIDIwIHRlc3R1c2VyMkBkb21haW4uY29tIDEgMCA0IHBhc3MgMSAwIDE2IHVzZXIxQGRvbWFpbi5jb20gMTYgdXNlcjFAZG9tYWluLmNvbQ==";
    {
        auto restoredChats = chatsDeserialise(serializedChats_BoostArchiveV10_ClassVersion_2);
        if (archiveLibraryVersion == Swift::BoostArchiveSkipVersion) {
            ASSERT_EQ(restoredChats.size(), 0);
        }
        else {
            ASSERT_EQ(restoredChats.size(), 4);

            EXPECT_FALSE(restoredChats[0].isMUC);
            EXPECT_EQ(restoredChats[0].jid, "swift@rooms.swift.im");
            EXPECT_EQ(restoredChats[0].chatName, "swift@rooms.swift.im");
            EXPECT_EQ(restoredChats[0].activity, "Some text 0");
            EXPECT_EQ(restoredChats[0].nick, "Nick Name");
            EXPECT_EQ(restoredChats[0].impromptuJIDs.size(), 0);
            EXPECT_EQ(restoredChats[0].password.get_value_or(""), "");
            EXPECT_EQ(restoredChats[0].inviteesNames.size(), 0);

            EXPECT_FALSE(restoredChats[1].isMUC);
            EXPECT_EQ(restoredChats[1].jid, "testuser1@domain.com");
            EXPECT_EQ(restoredChats[1].chatName, "swift@rooms2.swift.im");
            EXPECT_EQ(restoredChats[1].activity, "Some text 1");
            EXPECT_EQ(restoredChats[1].nick, "Nick Name");
            EXPECT_EQ(restoredChats[1].impromptuJIDs.size(), 0);
            EXPECT_EQ(restoredChats[1].password.get_value_or(""), "pass");
            EXPECT_EQ(restoredChats[1].inviteesNames.size(), 0);

            EXPECT_TRUE(restoredChats[2].isMUC);
            EXPECT_EQ(restoredChats[2].jid, "testuser2@domain.com");
            EXPECT_EQ(restoredChats[2].chatName, "room 2");
            EXPECT_EQ(restoredChats[2].activity, "Some text 2");
            EXPECT_EQ(restoredChats[2].nick, "Nick Name");
            ASSERT_EQ(restoredChats[2].impromptuJIDs.size(), 2);
            EXPECT_EQ(restoredChats[2].impromptuJIDs["testuser1@domain.com"], "testuser1@domain.com");
            EXPECT_EQ(restoredChats[2].impromptuJIDs["testuser2@domain.com"], "testuser2@domain.com");
            EXPECT_EQ(restoredChats[2].password.get_value_or(""), "");
            ASSERT_EQ(restoredChats[2].inviteesNames.size(), 1);
            EXPECT_EQ(restoredChats[2].inviteesNames["user1@domain.com"], "user1@domain.com");

            EXPECT_TRUE(restoredChats[3].isMUC);
            EXPECT_EQ(restoredChats[3].jid, "testuser3@domain.com");
            EXPECT_EQ(restoredChats[3].chatName, "room 3");
            EXPECT_EQ(restoredChats[3].activity, "Some text 2");
            EXPECT_EQ(restoredChats[3].nick, "Nick Name");
            ASSERT_EQ(restoredChats[3].impromptuJIDs.size(), 2);
            EXPECT_EQ(restoredChats[3].impromptuJIDs["testuser1@domain.com"], "testuser1@domain.com");
            EXPECT_EQ(restoredChats[3].impromptuJIDs["testuser2@domain.com"], "testuser2@domain.com");
            EXPECT_EQ(restoredChats[3].password.get_value_or(""), "pass");
            ASSERT_EQ(restoredChats[3].inviteesNames.size(), 1);
            EXPECT_EQ(restoredChats[3].inviteesNames["user1@domain.com"], "user1@domain.com");
        }
    }
}

