/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>

namespace Swift {
	class ReadBytestream;
	class S5BProxyRequest;

	class FileTransferManager {
		public:
			virtual ~FileTransferManager();
			
			virtual void startListeningOnPort(int port) = 0;

			virtual OutgoingFileTransfer::ref createOutgoingFileTransfer(const JID& to, const boost::filesystem::path& filepath, const std::string& description, boost::shared_ptr<ReadBytestream> bytestream) = 0;
			virtual OutgoingFileTransfer::ref createOutgoingFileTransfer(const JID& to, const std::string& filename, const std::string& description, const boost::uintmax_t sizeInBytes, const boost::posix_time::ptime& lastModified, boost::shared_ptr<ReadBytestream> bytestream) = 0;
			
			boost::signal<void (IncomingFileTransfer::ref)> onIncomingFileTransfer;
	};
}
