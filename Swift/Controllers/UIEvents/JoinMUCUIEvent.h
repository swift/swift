//Not used yet.

#pragma once

#include "Swiften/Base/String.h"

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class JoinMUCUIEvent : public UIEvent {
		public:
			JoinMUCUIEvent(const JID& jid, const String& contact) : jid_(jid), contact_(contact) {};
			String getContact() {return contact_;};
			JID getJID() {return jid_;};
		private:
			String contact_;
			JID jid_;
	};
}
