/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>
#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
	class RequestContactEditorUIEvent : public UIEvent {
		public:
			typedef boost::shared_ptr<RequestContactEditorUIEvent> ref;

			RequestContactEditorUIEvent(const JID& jid) : jid(jid) {
			}

			const JID& getJID() const {
				return jid;
			}

		private:
			JID jid;
	};
}
