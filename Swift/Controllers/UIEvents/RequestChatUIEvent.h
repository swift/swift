//Not used yet.

#pragma once

#include "Swiften/Base/String.h"

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class RequestChatUIEvent : public UIEvent {
		public:
			RequestChatUIEvent(const String& contact) : contact_(contact) {};
			String getContact() {return contact_;}
		private:
			String contact_;
	};
}
