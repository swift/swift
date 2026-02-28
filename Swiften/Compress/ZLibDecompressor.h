/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Compress/ZLibCodecompressor.h>

namespace Swift {
    class SWIFTEN_API ZLibDecompressor : public ZLibCodecompressor {
        public:
            ZLibDecompressor();
            virtual ~ZLibDecompressor();

            virtual int processZStream();
    };
}
