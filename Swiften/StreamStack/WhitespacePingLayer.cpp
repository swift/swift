/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/WhitespacePingLayer.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>

namespace Swift {

static const int TIMEOUT_MILLISECONDS = 60000;

WhitespacePingLayer::WhitespacePingLayer(TimerFactory* timerFactory) : isActive(false) {
    timer = timerFactory->createTimer(TIMEOUT_MILLISECONDS);
    timer->onTick.connect(boost::bind(&WhitespacePingLayer::handleTimerTick, this));
}

WhitespacePingLayer::~WhitespacePingLayer() {
    SWIFT_LOG_ASSERT(!isActive, debug) << "WhitespacePingLayer still active at destruction." << std::endl;
    if (isActive) {
        timer->stop();
    }
    timer->onTick.disconnect(boost::bind(&WhitespacePingLayer::handleTimerTick, this));
}

void WhitespacePingLayer::writeData(const SafeByteArray& data) {
    writeDataToChildLayer(data);
}

void WhitespacePingLayer::handleDataRead(const SafeByteArray& data) {
    writeDataToParentLayer(data);
}

void WhitespacePingLayer::handleTimerTick() {
    timer->stop();
    writeDataToChildLayer(createSafeByteArray(" "));
    timer->start();
}

void WhitespacePingLayer::setActive() {
    isActive = true;
    timer->start();
}

void WhitespacePingLayer::setInactive() {
    timer->stop();
    isActive = false;
}

}
