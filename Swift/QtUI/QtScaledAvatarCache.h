/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <QString>

namespace Swift {
    class QtScaledAvatarCache {
        public:
            QtScaledAvatarCache(int size);

            QString getScaledAvatarPath(const QString& path);

        private:
            int size;
    };
}
