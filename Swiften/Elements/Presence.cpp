/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Elements/Presence.h>

#include <Swiften/Elements/Priority.h>
#include <Swiften/Elements/Status.h>

namespace Swift {

Presence::Presence() : type_(Available) /*, showType_(Online)*/ {
}

Presence::Presence(const std::string& status) : type_(Available) {
	setStatus(status);
}

Presence::~Presence() {
}

int Presence::getPriority() const {
	boost::shared_ptr<Priority> priority(getPayload<Priority>());
	return (priority ? priority->getPriority() : 0);
}

void Presence::setPriority(int priority) {
	updatePayload(boost::make_shared<Priority>(priority));
}

std::string Presence::getStatus() const { 
	boost::shared_ptr<Status> status(getPayload<Status>());
	if (status) {
		return status->getText();
	}
	return "";
}

void Presence::setStatus(const std::string& status) { 
	updatePayload(boost::make_shared<Status>(status));
}


}
