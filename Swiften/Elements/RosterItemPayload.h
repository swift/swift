/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_RosterItemPayloadPayload_H
#define SWIFTEN_RosterItemPayloadPayload_H

#include <vector>

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class RosterItemPayload
	{
		public:
			enum Subscription { None, To, From, Both, Remove };

			RosterItemPayload() : subscription_(None), ask_(false) {}
			RosterItemPayload(const JID& jid, const String& name, Subscription subscription) : jid_(jid), name_(name), subscription_(subscription), ask_(false) { }
			
			void setJID(const JID& jid) { jid_ = jid; }
			const JID& getJID() const { return jid_; }

			void setName(const String& name) { name_ = name; }
			const String& getName() const { return name_; }

			void setSubscription(Subscription subscription) { subscription_ = subscription; }
			const Subscription& getSubscription() const { return subscription_; }

			void addGroup(const String& group) { groups_.push_back(group); }
			void setGroups(const std::vector<String>& groups) { groups_ = groups; }
			const std::vector<String>& getGroups() const { return groups_; }

			void setSubscriptionRequested() { ask_ = true; }
			bool getSubscriptionRequested() const { return ask_; }

			const String& getUnknownContent() const { return unknownContent_; }
			void addUnknownContent(const String& c) { 
				unknownContent_ += c;
			}

		private:
			JID jid_;
			String name_;
			Subscription subscription_;
			std::vector<String> groups_;
			bool ask_;
			String unknownContent_;
	};
}

#endif
