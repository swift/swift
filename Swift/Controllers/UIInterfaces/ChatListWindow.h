/*
 * Copyright (c) 2010-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <list>
#include <set>
#include <map>
#include <boost/shared_ptr.hpp>
#include <Swiften/MUC/MUCBookmark.h>
#include <Swiften/Elements/StatusShow.h>
#include <boost/filesystem/path.hpp>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/boost_bsignals.h>

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
						} else { /* compare the chat occupant lists */
							typedef std::map<std::string, JID> JIDMap;
							foreach (const JIDMap::value_type& jid, impromptuJIDs) {
								bool found = false;
								foreach (const JIDMap::value_type& otherJID, other.impromptuJIDs) {
									if (jid.second.toBare() == otherJID.second.toBare()) {
										found = true;
										break;
									}
								}
								if (!found) {
									return false;
								}
							}
							return true;
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
						typedef std::pair<std::string, JID> StringJIDPair;
						std::string title;
						foreach(StringJIDPair pair, impromptuJIDs) {
							if (title.empty()) {
								title += pair.first;
							} else {
								title += ", " + pair.first;
							}
						}
						return title;
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

			boost::signal<void (const MUCBookmark&)> onMUCBookmarkActivated;
			boost::signal<void (const Chat&)> onRecentActivated;
			boost::signal<void (const JID&)> onWhiteboardActivated;
			boost::signal<void ()> onClearRecentsRequested;
	};
}
