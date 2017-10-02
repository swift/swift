/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <mutex>

#include <boost/asio/io_service.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/Event.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
    class SWIFTEN_API BoostASIOEventLoop : public EventLoop {
        public:
            BoostASIOEventLoop(std::shared_ptr<boost::asio::io_service> ioService);
            virtual ~BoostASIOEventLoop();

        protected:
            void handleASIOEvent();

            virtual void eventPosted();

        private:
            std::shared_ptr<boost::asio::io_service> ioService_;

            bool isEventInASIOEventLoop_ = false;
            std::recursive_mutex isEventInASIOEventLoopMutex_;
    };
}
