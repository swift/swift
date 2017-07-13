/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>

#include <Swiften/FileTransfer/FileTransferManager.h>

namespace Swift {
    class S5BProxyRequest;
    class FileTransferOptions;

    class DummyFileTransferManager : public FileTransferManager {
        public:
            DummyFileTransferManager() : FileTransferManager() {
            }

            virtual OutgoingFileTransfer::ref createOutgoingFileTransfer(
                    const JID&,
                    const boost::filesystem::path&,
                    const std::string&,
                    std::shared_ptr<ReadBytestream>,
                    const FileTransferOptions&) override {
                return OutgoingFileTransfer::ref();
            }

            virtual OutgoingFileTransfer::ref createOutgoingFileTransfer(
                    const JID&,
                    const std::string&,
                    const std::string&,
                    const boost::uintmax_t,
                    const boost::posix_time::ptime&,
                    std::shared_ptr<ReadBytestream>,
                    const FileTransferOptions&) override {
                return OutgoingFileTransfer::ref();
            }

            virtual void addS5BProxy(std::shared_ptr<S5BProxyRequest>) {
            }

    };
}
