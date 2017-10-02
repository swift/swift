/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/StatusTracker.h>

#include <memory>

#include <Swiften/Elements/Idle.h>

namespace Swift {

StatusTracker::StatusTracker() {
    isAutoAway_ = false;
    queuedPresence_ = std::make_shared<Presence>();
}

std::shared_ptr<Presence> StatusTracker::getNextPresence() {
    std::shared_ptr<Presence> presence;
    if (isAutoAway_) {
        presence = std::make_shared<Presence>();
        presence->setShow(StatusShow::Away);
        presence->setStatus(queuedPresence_->getStatus());
        presence->addPayload(std::make_shared<Idle>(isAutoAwaySince_));
    } else {
        presence = queuedPresence_;
    }
    return presence;
}

void StatusTracker::setRequestedPresence(std::shared_ptr<Presence> presence) {
    isAutoAway_ = false;
    queuedPresence_ = presence;
//    if (presence->getType() == Presence::Unavailable) {
//        queuedPresence_ = std::make_shared<Presence>();
//    }
}

bool StatusTracker::goAutoAway(const int& seconds) {
    if (queuedPresence_->getShow() != StatusShow::Online) {
        return false;
    }
    isAutoAway_ = true;
    isAutoAwaySince_ = boost::posix_time::second_clock::universal_time() - boost::posix_time::seconds(seconds);
    return true;
}

bool StatusTracker::goAutoUnAway() {
    if (!isAutoAway_) {
        return false;
    }
    isAutoAway_ = false;
    return true;
}

}
