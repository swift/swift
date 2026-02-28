/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

namespace Swift {
    class QtCachedImageScaler {
        public:
            QtCachedImageScaler();

            boost::filesystem::path getScaledImage(const boost::filesystem::path& image, int size);
    };
}

