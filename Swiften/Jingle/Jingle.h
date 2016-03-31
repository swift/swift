/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Elements/JingleContentPayload.h>

namespace Swift {
    namespace Jingle {
        template<typename T>
        JingleContentPayload::ref getContentWithDescription(const std::vector<JingleContentPayload::ref>& contents) {
            for (size_t i = 0; i < contents.size(); ++i) {
                if (contents[i]->getDescription<T>()) {
                    return contents[i];
                }
            }
            return JingleContentPayload::ref();
        }
    }
}
