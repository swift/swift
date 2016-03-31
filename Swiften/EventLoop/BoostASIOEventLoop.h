/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/Event.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
    class SWIFTEN_API BoostASIOEventLoop : public EventLoop {
        public:
            BoostASIOEventLoop(boost::shared_ptr<boost::asio::io_service> ioService);
            virtual ~BoostASIOEventLoop();

        protected:
            void handleASIOEvent();

            virtual void eventPosted();

        private:
            boost::shared_ptr<boost::asio::io_service> ioService_;

            bool isEventInASIOEventLoop_;
            boost::recursive_mutex isEventInASIOEventLoopMutex_;
    };
}
