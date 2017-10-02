/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/FileTransfer.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>

namespace Swift {
    class WriteBytestream;
    class JID;

    /**
     * @brief The IncomingFileTransfer abstract class is the general interface in Swiften
     * for incoming file transfers.
     */
    class SWIFTEN_API IncomingFileTransfer : public FileTransfer {
        public:
            typedef std::shared_ptr<IncomingFileTransfer> ref;

            virtual ~IncomingFileTransfer();

            virtual void accept(
                    std::shared_ptr<WriteBytestream>,
                    const FileTransferOptions& = FileTransferOptions()) = 0;

            virtual const JID& getSender() const = 0;
            virtual const JID& getRecipient() const = 0;
    };
}
