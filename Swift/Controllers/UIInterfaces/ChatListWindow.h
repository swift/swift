/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <list>
#include <map>
#include <memory>
#include <set>


#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Elements/StatusShow.h>
#include <Swiften/MUC/MUCBookmark.h>

namespace Swift {
    class ChatListWindow {
        public:
            class Chat {
                public:
                    Chat() : statusType(StatusShow::None), isMUC(false), unreadCount(0), isPrivateMessage(false) {}
                    Chat(const JID& jid, const std::string& chatName, const std::string& activity, int unreadCount, StatusShow::Type statusType, const boost::filesystem::path& avatarPath, bool isMUC, bool isPrivateMessage = false, const std::string& nick = "", const boost::optional<std::string> password = boost::optional<std::string>())
                    : jid(jid), chatName(chatName), activity(activity), statusType(statusType), isMUC(isMUC), nick(nick), password(password), unreadCount(unreadCount), avatarPath(avatarPath), isPrivateMessage(isPrivateMessage) {}
                    /** Assume that nicks and other transient features aren't important for equality */
                    bool operator==(const Chat& other) const {
                        if (impromptuJIDs.empty()) {
                            return jid.toBare() == other.jid.toBare()
                                    && isMUC == other.isMUC;
                        }
                        if (chatName == other.chatName) {
                            return true;
                        }
                        else { /* compare the chat occupant lists */
                            for (const auto& jid : impromptuJIDs) {
                                bool found = false;
                                for (const auto& otherJID : other.impromptuJIDs) {
                                    if (jid.second.toBare() == otherJID.second.toBare()) {
                                        found = true;
                                        break;
                                    }
                                }
                                if (!found) {
                                    return false;
                                }
                            }
                            return key_compare(inviteesNames, other.inviteesNames);
                        }
                    }
                    void setUnreadCount(int unread) {
                        unreadCount = unread;
                    }
                    void setStatusType(StatusShow::Type type) {
                        statusType = type;
                    }
                    void setAvatarPath(const boost::filesystem::path& path) {
                        avatarPath = path;
                    }
                    std::string getImpromptuTitle() const {
                        std::set<std::string> participants;
                        std::map<JID, std::string> participantsMap;

                        for (auto& pair : inviteesNames) {
                            if (!pair.second.empty()) {
                                participantsMap[pair.first] = pair.second;
                            }
                            else {
                                participantsMap[pair.first] = pair.first.toString();
                            }
                        }
                        for (auto& pair : impromptuJIDs) {
                            participantsMap[pair.second] = pair.first;
                        }
                        for (auto& participant : participantsMap) {
                            participants.insert(participant.second);
                        }
                        return boost::algorithm::join(participants, ", ");
                    }
                    std::string getTitle() const {
                        std::string title = getImpromptuTitle();
                        return title.empty() ? chatName : title;
                    }
                    JID jid;
                    std::string chatName;
                    std::string activity;
                    StatusShow::Type statusType;
                    bool isMUC;
                    std::string nick;
                    boost::optional<std::string> password;
                    int unreadCount;
                    boost::filesystem::path avatarPath;
                    std::map<std::string, JID> impromptuJIDs;
                    std::map<JID, std::string> inviteesNames;
                    bool isPrivateMessage;
            };
            virtual ~ChatListWindow();

            virtual void setBookmarksEnabled(bool enabled) = 0;
            virtual void addMUCBookmark(const MUCBookmark& bookmark) = 0;
            virtual void addWhiteboardSession(const ChatListWindow::Chat& chat) = 0;
            virtual void removeWhiteboardSession(const JID& jid) = 0;
            virtual void removeMUCBookmark(const MUCBookmark& bookmark) = 0;
            virtual void setRecents(const std::list<Chat>& recents) = 0;
            virtual void setUnreadCount(int unread) = 0;
            virtual void clearBookmarks() = 0;
            virtual void setOnline(bool isOnline) = 0;

            boost::signals2::signal<void (const MUCBookmark&)> onMUCBookmarkActivated;
            boost::signals2::signal<void (const Chat&)> onRecentActivated;
            boost::signals2::signal<void (const JID&)> onWhiteboardActivated;
            boost::signals2::signal<void ()> onClearRecentsRequested;
    };
}
