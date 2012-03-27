/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

namespace Swift {

	class MUCInviteEvent : public StanzaEvent {
	public:
		typedef boost::shared_ptr<MUCInviteEvent> ref;

	public:
		MUCInviteEvent(const JID& inviter, const JID& roomJID, const std::string& reason, const std::string& password, bool direct) : inviter_(inviter), roomJID_(roomJID), reason_(reason), password_(password), direct_(direct) {}

		const JID& getInviter() const { return inviter_; }
		const JID& getRoomJID() const { return roomJID_; }
		const std::string& getReason() const { return reason_; }
		const std::string& getPassword() const { return password_; }
		bool getDirect() const { return direct_; }

	private:
		JID inviter_;
		JID roomJID_;
		std::string reason_;
		std::string password_;
		bool direct_;
	};
}
