/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <string>
#include <Swiften/JID/JID.h>

namespace Swift {
	class Client;

	class MUCOccupant {
		public:
			enum Role {Moderator, Participant, Visitor, NoRole};
			enum Affiliation {Owner, Admin, Member, Outcast, NoAffiliation};

			MUCOccupant(const std::string &nick, Role role, Affiliation affiliation);
			MUCOccupant(const MUCOccupant& other);
			~MUCOccupant();

			std::string getNick() const;
			Role getRole() const;
			Affiliation getAffiliation() const;
			boost::optional<JID> getRealJID() const;
			void setRealJID(const JID& jid);
			void setNick(const std::string& nick);

		private:
			std::string nick_;
			Role role_;
			Affiliation affiliation_;
			boost::optional<JID> realJID_;
			/* If you add a field, remember to update the const copy constructor */
	};
}

