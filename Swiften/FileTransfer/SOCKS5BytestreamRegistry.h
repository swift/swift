/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include <string>
#include "Swiften/FileTransfer/ReadBytestream.h"

namespace Swift {
	class SOCKS5BytestreamRegistry {
		public:
			SOCKS5BytestreamRegistry();

			boost::shared_ptr<ReadBytestream> getBytestream(const std::string& destination) const;
			void addBytestream(const std::string& destination, boost::shared_ptr<ReadBytestream> byteStream);
			void removeBytestream(const std::string& destination);

		private:
			typedef std::map<std::string, boost::shared_ptr<ReadBytestream> > BytestreamMap;
			BytestreamMap byteStreams;
	};
}

