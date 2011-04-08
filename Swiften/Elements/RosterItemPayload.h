/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <Swiften/JID/JID.h>

namespace Swift {
	class RosterItemPayload {
		public:
			enum Subscription { None, To, From, Both, Remove };

			RosterItemPayload() : subscription_(None), ask_(false) {}
			RosterItemPayload(const JID& jid, const std::string& name, Subscription subscription, const std::vector<std::string>& groups = std::vector<std::string>()) : jid_(jid), name_(name), subscription_(subscription), groups_(groups), ask_(false) { }
			
			void setJID(const JID& jid) { jid_ = jid; }
			const JID& getJID() const { return jid_; }

			void setName(const std::string& name) { name_ = name; }
			const std::string& getName() const { return name_; }

			void setSubscription(Subscription subscription) { subscription_ = subscription; }
			const Subscription& getSubscription() const { return subscription_; }

			void addGroup(const std::string& group) { groups_.push_back(group); }
			void setGroups(const std::vector<std::string>& groups) { groups_ = groups; }
			const std::vector<std::string>& getGroups() const { return groups_; }

			void setSubscriptionRequested() { ask_ = true; }
			bool getSubscriptionRequested() const { return ask_; }

			const std::string& getUnknownContent() const { return unknownContent_; }
			void addUnknownContent(const std::string& c) { 
				unknownContent_ += c;
			}

		private:
			JID jid_;
			std::string name_;
			Subscription subscription_;
			std::vector<std::string> groups_;
			bool ask_;
			std::string unknownContent_;
	};
}
