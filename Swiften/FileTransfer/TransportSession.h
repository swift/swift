/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/FileTransferError.h>

namespace Swift {
    class SWIFTEN_API TransportSession {
        public:
            virtual ~TransportSession();

            virtual void start() = 0;
            virtual void stop() = 0;

            boost::signals2::signal<void (size_t)> onBytesSent;
            boost::signals2::signal<void (boost::optional<FileTransferError>)> onFinished;
    };
}
