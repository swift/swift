/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class SWIFTEN_API ZLibCodecompressor {
        public:
            ZLibCodecompressor();
            virtual ~ZLibCodecompressor();

            SafeByteArray process(const SafeByteArray& data);
            virtual int processZStream() = 0;

        protected:
            struct Private;
            boost::shared_ptr<Private> p;
    };
}
