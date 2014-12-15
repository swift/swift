/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>
#include <set>

#include <Swiften/Base/API.h>
#include <Swiften/Base/IDGenerator.h>

namespace Swift {
	class SOCKS5BytestreamServerSession;

	class SWIFTEN_API SOCKS5BytestreamRegistry {
		public:
			SOCKS5BytestreamRegistry();

			void setHasBytestream(const std::string& destination, bool);
			bool hasBytestream(const std::string& destination) const;

			/**
			 * Generate a new session ID to use for new S5B streams.
			 */
			std::string generateSessionID();

		private:
			std::set<std::string> availableBytestreams;
			IDGenerator idGenerator;
	};
}
