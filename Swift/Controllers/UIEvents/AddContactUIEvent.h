#pragma once

#include "Swiften/Base/String.h"

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class AddContactUIEvent : public UIEvent {
		public:
			AddContactUIEvent(const JID& jid, const String& name) : jid_(jid), name_(name) {};
			String getName() {return name_;};
			JID getJID() {return jid_;};
		private:
			JID jid_;
			String name_;
	};
}
