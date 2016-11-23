/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/EventLoop.h>

#include <algorithm>
#include <cassert>
#include <vector>

#include <boost/bind.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/Log.h>

namespace lambda = boost::lambda;

namespace Swift {

inline void invokeCallback(const Event& event) {
    try {
        assert(!event.callback.empty());
        event.callback();
    }
    catch (const std::exception& e) {
        SWIFT_LOG(error) << "Uncaught exception in event loop: " << e.what() << std::endl;
    }
    catch (...) {
        SWIFT_LOG(error) << "Uncaught non-exception in event loop" << std::endl;
    }
}

EventLoop::EventLoop() : nextEventID_(0), handlingEvents_(false) {
}

EventLoop::~EventLoop() {
}

void EventLoop::handleNextEvents() {
    const int eventsBatched = 100;
    // If handleNextEvents is already in progress, e.g. in case of a recursive call due to
    // the event loop implementation, then do no handle further events. Instead call
    // eventPosted() to continue event handling later.
    bool callEventPosted = handlingEvents_;
    if (!handlingEvents_) {
        handlingEvents_ = true;
        std::unique_lock<std::recursive_mutex> lock(removeEventsMutex_);
        {
            std::vector<Event> nextEvents;
            {
                std::unique_lock<std::recursive_mutex> lock(eventsMutex_);
                for (int n = 0; ((n < eventsBatched) && !events_.empty()); n++) {
                    nextEvents.push_back(events_.front());
                    events_.pop_front();
                }
                callEventPosted = !events_.empty();
            }
            if (!nextEvents.empty()) {
                for (const auto& event : nextEvents) {
                    invokeCallback(event);
                }
            }
        }
        handlingEvents_ = false;
    }

    if (callEventPosted) {
        eventPosted();
    }
}

void EventLoop::postEvent(boost::function<void ()> callback, std::shared_ptr<EventOwner> owner) {
    Event event(owner, callback);
    bool callEventPosted = false;
    {
        std::unique_lock<std::recursive_mutex> lock(eventsMutex_);

        callEventPosted = events_.empty();

        event.id = nextEventID_;
        nextEventID_++;
        events_.push_back(event);
    }
    if (callEventPosted) {
        eventPosted();
    }
}

void EventLoop::removeEventsFromOwner(std::shared_ptr<EventOwner> owner) {
    std::unique_lock<std::recursive_mutex> removeLock(removeEventsMutex_, std::defer_lock);
    std::unique_lock<std::recursive_mutex> eventsLock(eventsMutex_, std::defer_lock);

    std::lock(removeLock, eventsLock);

    events_.remove_if(lambda::bind(&Event::owner, lambda::_1) == owner);
}

}
