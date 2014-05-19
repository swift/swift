/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class RequestAdHocWithJIDUIEvent : public UIEvent {
		public:
			RequestAdHocWithJIDUIEvent(const JID& jid, const std::string& node) : jid_(jid), node_(node) {}
			JID getJID() const { return jid_; }
			std::string getNode() const { return node_; }
		private:
			JID jid_;
			std::string node_;
	};
}
