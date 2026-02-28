/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Compress/ZLibCodecompressor.h>

namespace Swift {
    class SWIFTEN_API ZLibCompressor : public ZLibCodecompressor {
        public:
            ZLibCompressor();
            virtual ~ZLibCompressor();

            virtual int processZStream();

        private:
            static const int COMPRESSION_LEVEL = 9;
    };
}
