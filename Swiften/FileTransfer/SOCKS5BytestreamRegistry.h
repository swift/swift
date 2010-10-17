/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/FileTransfer/ReadBytestream.h"

namespace Swift {
	class SOCKS5BytestreamRegistry {
		public:
			SOCKS5BytestreamRegistry();

			boost::shared_ptr<ReadBytestream> getBytestream(const String& destination) const;
			void addBytestream(const String& destination, boost::shared_ptr<ReadBytestream> byteStream);
			void removeBytestream(const String& destination);

		private:
			typedef std::map<String, boost::shared_ptr<ReadBytestream> > BytestreamMap;
			BytestreamMap byteStreams;
	};
}

