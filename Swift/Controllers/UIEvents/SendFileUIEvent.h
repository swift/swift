/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/JID/JID.h>
#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
	class SendFileUIEvent : public UIEvent {
		public:
			typedef boost::shared_ptr<SendFileUIEvent> ref;

			SendFileUIEvent(const JID& jid, const std::string& filename) : jid(jid), filename(filename) {
			}

			const JID& getJID() const {
				return jid;
			}
			
			const std::string& getFilename() const {
				return filename;
			}

		private:
			JID jid;
			std::string filename;
	};
}
