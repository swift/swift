/*
 * Copyright (c) 2011-2016 Isode Limited.
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
            for (const auto& content : contents) {
                if (content->getDescription<T>()) {
                    return content;
                }
            }
            return JingleContentPayload::ref();
        }
    }
}
