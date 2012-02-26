/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>
#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {

class ShowProfileForRosterItemUIEvent : public UIEvent {
	public:
		typedef boost::shared_ptr<ShowProfileForRosterItemUIEvent> ref;
	public:
		ShowProfileForRosterItemUIEvent(const JID& jid) : jid_(jid) {}
		virtual ~ShowProfileForRosterItemUIEvent() {}
		JID getJID() const {return jid_;}
	private:
		JID jid_;
};

}
