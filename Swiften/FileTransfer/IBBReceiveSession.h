/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/FileTransfer/FileTransferError.h>
#include <Swiften/FileTransfer/WriteBytestream.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class IQRouter;

    class SWIFTEN_API IBBReceiveSession {
        public:
            IBBReceiveSession(
                    const std::string& id,
                    const JID& from,
                    const JID& to,
                    unsigned long long size,
                    std::shared_ptr<WriteBytestream> bytestream,
                    IQRouter* router);
            ~IBBReceiveSession();

            void start();
            void stop();

            const JID& getSender() const {
                return from;
            }

            const JID& getReceiver() const {
                return to;
            }

            boost::signals2::signal<void (boost::optional<FileTransferError>)> onFinished;

        private:
            bool handleSetRequest(const JID& from, const JID& to, const std::string& id, IBB::ref payload);
            void finish(boost::optional<FileTransferError>);

        private:
            class IBBResponder;
            friend class IBBResponder;

            std::string id;
            JID from;
            JID to;
            unsigned long long size;
            std::shared_ptr<WriteBytestream> bytestream;
            IQRouter* router;
            IBBResponder* responder;
            bool active;
    };
}
