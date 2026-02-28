/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/WriteBytestream.h>

namespace Swift {
    class SWIFTEN_API ByteArrayWriteBytestream : public WriteBytestream {
        public:
            ByteArrayWriteBytestream() {
            }

            virtual bool write(const std::vector<unsigned char>& bytes) {
                data.insert(data.end(), bytes.begin(), bytes.end());
                onWrite(bytes);
                return true;
            }

            const std::vector<unsigned char>& getData() const {
                return data;
            }

        private:
            std::vector<unsigned char> data;
    };
}
