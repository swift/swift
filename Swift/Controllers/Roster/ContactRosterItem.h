/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include "Swiften/JID/JID.h"
#include "Swift/Controllers/Roster/RosterItem.h"
#include "Swiften/Elements/StatusShow.h"
#include "Swiften/Elements/Presence.h"

#include <map>
#include <set>
#include <boost/bind.hpp>
#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

namespace Swift {

class GroupRosterItem;
class ContactRosterItem : public RosterItem {
	public:
		enum Feature {
			FileTransferFeature,
		};
		
	public:
		ContactRosterItem(const JID& jid, const JID& displayJID, const std::string& name, GroupRosterItem* parent);
		virtual ~ContactRosterItem();

		StatusShow::Type getStatusShow() const;
		StatusShow::Type getSimplifiedStatusShow() const;
		std::string getStatusText() const;
		void setAvatarPath(const std::string& path);
		const std::string& getAvatarPath() const;
		const JID& getJID() const;
		void setDisplayJID(const JID& jid);
		const JID& getDisplayJID() const;
		void applyPresence(const std::string& resource, boost::shared_ptr<Presence> presence);
		void clearPresence();
		void calculateShownPresence();
		const std::vector<std::string>& getGroups() const;
		/** Only used so a contact can know about the groups it's in*/
		void addGroup(const std::string& group);
		void removeGroup(const std::string& group);
		
		void setSupportedFeatures(const std::set<Feature>& features);
		bool supportsFeature(Feature feature) const;
	private:
		JID jid_;
		JID displayJID_;
		std::string avatarPath_;
		std::map<std::string, boost::shared_ptr<Presence> > presences_;
		boost::shared_ptr<Presence> offlinePresence_;
		boost::shared_ptr<Presence> shownPresence_;
		std::vector<std::string> groups_;
		std::set<Feature> features_;
};

}

