/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <set>

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class AddContactUIEvent : public UIEvent {
		public:
			AddContactUIEvent(const JID& jid, const std::string& name, const std::set<std::string>& groups) : jid_(jid), name_(name), groups_(groups) {};

			const std::string& getName() const {
				return name_;
			};

			const JID& getJID() const {
				return jid_;
			};

			const std::set<std::string>& getGroups() const {
				return groups_;
			}

		private:
			JID jid_;
			std::string name_;
			std::set<std::string> groups_;
	};
}
