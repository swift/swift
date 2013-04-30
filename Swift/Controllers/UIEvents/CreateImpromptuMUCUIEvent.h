/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {

class CreateImpromptuMUCUIEvent : public UIEvent {
	public:
		CreateImpromptuMUCUIEvent(const std::vector<JID>& jids, const JID& roomJID = JID(), const std::string reason = "") : jids_(jids), roomJID_(roomJID), reason_(reason) { }

		std::vector<JID> getJIDs() const { return jids_; }
		JID getRoomJID() const { return roomJID_; }
		std::string getReason() const { return reason_; }
	private:
		std::vector<JID> jids_;
		JID roomJID_;
		std::string reason_;
};

}
