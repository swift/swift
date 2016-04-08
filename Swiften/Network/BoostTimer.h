/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <mutex>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/scoped_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/Timer.h>

namespace Swift {
    class EventLoop;

    class SWIFTEN_API BoostTimer : public Timer, public EventOwner, public std::enable_shared_from_this<BoostTimer> {
        public:
            typedef std::shared_ptr<BoostTimer> ref;

            virtual ~BoostTimer();

            static ref create(int milliseconds, std::shared_ptr<boost::asio::io_service> service, EventLoop* eventLoop) {
                return ref(new BoostTimer(milliseconds, service, eventLoop));
            }

            virtual void start();
            virtual void stop();

        private:
            BoostTimer(int milliseconds, std::shared_ptr<boost::asio::io_service> service, EventLoop* eventLoop);

            void handleTimerTick(const boost::system::error_code& error);

        private:
            int timeout;
            std::shared_ptr<boost::asio::io_service> ioService;
            boost::scoped_ptr<boost::asio::deadline_timer> timer;
            std::mutex timerMutex;
            EventLoop* eventLoop;
            bool shuttingDown;
    };
}
