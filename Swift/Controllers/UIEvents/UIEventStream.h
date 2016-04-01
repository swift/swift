/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/boost_bsignals.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class UIEventStream {
        public:
            boost::signal<void (std::shared_ptr<UIEvent>)> onUIEvent;

            void send(std::shared_ptr<UIEvent> event) {
                onUIEvent(event);
            }
    };
}
