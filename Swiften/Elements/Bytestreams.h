/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/Shared.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class Bytestreams : public Payload, public Shared<Bytestreams> {
		public:
			struct StreamHost {
				StreamHost(const String& host = "", const JID& jid = JID(), int port = -1) : host(host), jid(jid), port(port) {}

				String host;
				JID jid;
				int port;
			};

			Bytestreams() {}

			const String& getStreamID() const {
				return id;
			}

			void setStreamID(const String& id) {
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
			String id;
			boost::optional<JID> usedStreamHost;
			std::vector<StreamHost> streamHosts;
	};
}
