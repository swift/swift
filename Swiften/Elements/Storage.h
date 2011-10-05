/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Elements/Payload.h>
#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Base/SafeString.h>

namespace Swift {
	class Storage : public Payload {
		public:
			struct Room {
				Room() : autoJoin(false) {}

				std::string name;
				JID jid;
				bool autoJoin;
				std::string nick;
				boost::optional<std::string> password;
			};

			struct URL {
				URL() {}

				std::string name;
				std::string url;
			};

			Storage() {
			}

			
			void clearRooms() {
				rooms.clear();
			}

			const std::vector<Room>& getRooms() const {
				return rooms;
			}

			void addRoom(const Room& room) {
				rooms.push_back(room);
			}

			const std::vector<URL>& getURLs() const {
				return urls;
			}

			void addURL(const URL& url) {
				urls.push_back(url);
			}

		private:
			std::vector<Room> rooms;
			std::vector<URL> urls;
	};
}
