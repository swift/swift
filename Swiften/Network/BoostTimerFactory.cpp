/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BoostTimerFactory.h>

#include <Swiften/Network/BoostTimer.h>

namespace Swift {

BoostTimerFactory::BoostTimerFactory(std::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : ioService(ioService), eventLoop(eventLoop) {
}

std::shared_ptr<Timer> BoostTimerFactory::createTimer(int milliseconds) {
    return BoostTimer::create(milliseconds, ioService, eventLoop);
}

}
