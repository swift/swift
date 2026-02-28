/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
    template<typename T> class StartStopper {
        public:
            StartStopper(T* target) : target(target) {
                target->start();
            }

            ~StartStopper() {
                target->stop();
            }

        private:
            T* target;
    };
}
