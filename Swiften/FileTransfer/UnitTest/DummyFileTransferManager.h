/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/FileTransfer/FileTransferManager.h>

namespace Swift {
	class DummyFileTransferManager : public FileTransferManager {
		public:
			DummyFileTransferManager() : FileTransferManager() {
			}

			virtual OutgoingFileTransfer::ref createOutgoingFileTransfer(const JID&, const boost::filesystem::path&, const std::string&, boost::shared_ptr<ReadBytestream>) {
				return OutgoingFileTransfer::ref();
			}

			virtual OutgoingFileTransfer::ref createOutgoingFileTransfer(const JID&, const std::string&, const std::string&, const boost::uintmax_t, const boost::posix_time::ptime&, boost::shared_ptr<ReadBytestream>) {
				return OutgoingFileTransfer::ref();
			}

			virtual void startListeningOnPort(int) {
			}

			virtual void addS5BProxy(boost::shared_ptr<S5BProxyRequest>) {
			}

	};
}
