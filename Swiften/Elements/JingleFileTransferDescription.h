/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/JingleFileTransferFileInfo.h>

namespace Swift {
    class SWIFTEN_API JingleFileTransferDescription : public JingleDescription {
        public:
            typedef std::shared_ptr<JingleFileTransferDescription> ref;

            void setFileInfo(const JingleFileTransferFileInfo& fileInfo) {
                fileInfo_ = fileInfo;
            }

            const JingleFileTransferFileInfo& getFileInfo() {
                return fileInfo_;
            }

        private:
            JingleFileTransferFileInfo fileInfo_;
    };
}
