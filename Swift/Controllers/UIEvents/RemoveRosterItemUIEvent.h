/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/JID/JID.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {

class RemoveRosterItemUIEvent : public UIEvent {
	public:
		RemoveRosterItemUIEvent(const JID& jid) : jid_(jid) {};
		virtual ~RemoveRosterItemUIEvent() {};
		JID getJID() {return jid_;};
	private:
		JID jid_;

};

}
