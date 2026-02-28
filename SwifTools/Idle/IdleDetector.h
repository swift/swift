/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

namespace Swift {
    class IdleDetector {
        public:
            IdleDetector() : idle(false), idleTimeSeconds(300) {}
            virtual ~IdleDetector();

            void setIdleTimeSeconds(int time) {
                idleTimeSeconds = time;
            }

            int getIdleTimeSeconds() const {
                return idleTimeSeconds;
            }

            virtual bool isIdle() const {
                return idle;
            }

            boost::signals2::signal<void (bool /* isIdle */)> onIdleChanged;

            void setIdle(bool b) {
                if (b != idle) {
                    idle = b;
                    onIdleChanged(b);
                }
            }

        private:
            bool idle;
            int idleTimeSeconds;
    };
}
