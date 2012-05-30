/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
	class AcceptWhiteboardSessionUIEvent : public UIEvent {
		typedef boost::shared_ptr<AcceptWhiteboardSessionUIEvent> ref;
	public:
		AcceptWhiteboardSessionUIEvent(const JID& jid) : jid_(jid) {}
		const JID& getContact() const {return jid_;}
	private:
		JID jid_;
	};
}
