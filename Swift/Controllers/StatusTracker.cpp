/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/StatusTracker.h"

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

StatusTracker::StatusTracker() {
	isAutoAway_ = false;
	queuedPresence_ = boost::make_shared<Presence>();
}

boost::shared_ptr<Presence> StatusTracker::getNextPresence() {
	boost::shared_ptr<Presence> presence;
	if (isAutoAway_) {
		presence = boost::make_shared<Presence>();
		presence->setShow(StatusShow::Away);
		presence->setStatus(queuedPresence_->getStatus());
	} else {
		presence = queuedPresence_;
	}
	return presence;
}

void StatusTracker::setRequestedPresence(boost::shared_ptr<Presence> presence) {
	isAutoAway_ = false;
	queuedPresence_ = presence;
//	if (presence->getType() == Presence::Unavailable) {
//		queuedPresence_ = boost::make_shared<Presence>();
//	}
}

bool StatusTracker::goAutoAway() {
	if (queuedPresence_->getShow() != StatusShow::Online) {
		return false;
	}
	isAutoAway_ = true;
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
