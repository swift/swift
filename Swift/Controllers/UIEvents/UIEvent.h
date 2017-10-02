/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

namespace Swift {
    class UIEvent {
        public:
            typedef std::shared_ptr<UIEvent> ref;

            virtual ~UIEvent();
    };
}
