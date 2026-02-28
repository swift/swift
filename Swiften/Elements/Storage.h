/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API Storage : public Payload {
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
