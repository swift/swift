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
    class SWIFTEN_API ReadBytestream {
        public:
            virtual ~ReadBytestream();

            /**
             * Return an empty vector if no more data is available.
             * Use onDataAvailable signal for signaling there is data available again.
             */
            virtual std::shared_ptr< std::vector<unsigned char> > read(size_t size) = 0;

            virtual bool isFinished() const = 0;

        public:
            boost::signals2::signal<void ()> onDataAvailable;
            boost::signals2::signal<void (const std::vector<unsigned char>&)> onRead;
    };
}
