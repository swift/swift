/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/FileTransfer.h>

namespace Swift {
    class SWIFTEN_API OutgoingFileTransfer : public FileTransfer {
        public:
            typedef std::shared_ptr<OutgoingFileTransfer> ref;
        public:
            virtual ~OutgoingFileTransfer();

            virtual void start() = 0;
    };
}
