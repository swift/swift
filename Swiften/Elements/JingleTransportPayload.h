/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class JingleTransportPayload : public Payload {
		public:
			void setSessionID(const std::string& id) {
				sessionID = id;
			}

			const std::string& getSessionID() const {
				return sessionID;
			}

		public:
			typedef boost::shared_ptr<JingleTransportPayload> ref;

		private:
			std::string sessionID;
	};
}
