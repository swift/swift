/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API WriteBytestream {
        public:
            typedef std::shared_ptr<WriteBytestream> ref;

            virtual ~WriteBytestream();

            /**
             * Write data from vector argument to bytestream.
             *
             * On success true is returned and \ref onWrite is called. On failure false is returned.
             */
            virtual bool write(const std::vector<unsigned char>&) = 0;
            virtual void close() {}

            boost::signals2::signal<void (const std::vector<unsigned char>&)> onWrite;
    };
}
