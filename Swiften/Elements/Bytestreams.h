/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API Bytestreams : public Payload {
        public:
            typedef std::shared_ptr<Bytestreams> ref;

            struct StreamHost {
                StreamHost(const std::string& host = "", const JID& jid = JID(), unsigned short port = 0) : host(host), jid(jid), port(port) {}

                std::string host;
                JID jid;
                unsigned short port;
            };

            Bytestreams() {}

            const std::string& getStreamID() const {
                return id;
            }

            void setStreamID(const std::string& id) {
                this->id = id;
            }

            const boost::optional<JID>& getUsedStreamHost() const {
                return usedStreamHost;
            }

            void setUsedStreamHost(const JID& host) {
                usedStreamHost = host;
            }

            const std::vector<StreamHost>& getStreamHosts() const {
                return streamHosts;
            }

            void addStreamHost(const StreamHost& streamHost) {
                streamHosts.push_back(streamHost);
            }

        private:
            std::string id;
            boost::optional<JID> usedStreamHost;
            std::vector<StreamHost> streamHosts;
    };
}
