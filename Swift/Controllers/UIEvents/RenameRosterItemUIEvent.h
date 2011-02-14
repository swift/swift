/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swiften/MUC/MUCBookmark.h"

namespace Swift {
	class RenameRosterItemUIEvent : public UIEvent {
		public:
			RenameRosterItemUIEvent(const JID& jid, const std::string& newName) : jid_(jid), newName_(newName) {}

			const JID& getJID() const {return jid_;}
			const std::string& getNewName() const {return newName_;}

		private:
			JID jid_;
			std::string newName_;
	};
}
