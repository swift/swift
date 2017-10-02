/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/StreamStack/StreamLayer.h>

namespace Swift {
    class Timer;
    class TimerFactory;

    class SWIFTEN_API WhitespacePingLayer : public StreamLayer, boost::noncopyable {
        public:
            WhitespacePingLayer(TimerFactory* timerFactory);
            virtual ~WhitespacePingLayer();

            void setActive();
            void setInactive();
            void writeData(const SafeByteArray& data);
            void handleDataRead(const SafeByteArray& data);

            bool getIsActive() const {
                return isActive;
            }

        private:
            void handleTimerTick();

        private:
            bool isActive;
            std::shared_ptr<Timer> timer;
    };
}
