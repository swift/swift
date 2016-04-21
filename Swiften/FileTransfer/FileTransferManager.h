/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class ReadBytestream;

    class SWIFTEN_API FileTransferManager {
        public:
            virtual ~FileTransferManager();

            virtual OutgoingFileTransfer::ref createOutgoingFileTransfer(
                    const JID& to,
                    const boost::filesystem::path& filepath,
                    const std::string& description,
                    std::shared_ptr<ReadBytestream> bytestream,
                    const FileTransferOptions& = FileTransferOptions()) = 0;
            virtual OutgoingFileTransfer::ref createOutgoingFileTransfer(
                    const JID& to,
                    const std::string& filename,
                    const std::string& description,
                    const boost::uintmax_t sizeInBytes,
                    const boost::posix_time::ptime& lastModified,
                    std::shared_ptr<ReadBytestream> bytestream,
                    const FileTransferOptions& = FileTransferOptions()) = 0;

            boost::signals2::signal<void (IncomingFileTransfer::ref)> onIncomingFileTransfer;
    };
}
