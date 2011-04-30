/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class Bytestreams : public Payload {
		public:
			typedef boost::shared_ptr<Bytestreams> ref;

			struct StreamHost {
				StreamHost(const std::string& host = "", const JID& jid = JID(), int port = -1) : host(host), jid(jid), port(port) {}

				std::string host;
				JID jid;
				int port;
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
