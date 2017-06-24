/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/BoostTimer.h>
#include <Swiften/Network/TimerFactory.h>

namespace Swift {
    class EventLoop;

    class SWIFTEN_API BoostTimerFactory : public TimerFactory {
        public:
            BoostTimerFactory(std::shared_ptr<boost::asio::io_service>, EventLoop* eventLoop);

            virtual std::shared_ptr<Timer> createTimer(int milliseconds);

        private:
            std::shared_ptr<boost::asio::io_service> ioService;
            EventLoop* eventLoop;
    };
}
