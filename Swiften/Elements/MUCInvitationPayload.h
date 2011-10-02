/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class MUCInvitationPayload : public Payload {
		public:
			typedef boost::shared_ptr<MUCInvitationPayload> ref;
			MUCInvitationPayload() : continuation_(false) {
			}

			void setIsContinuation(bool b) {
				continuation_ = b;
			}

			bool getIsContinuation() const {
				return continuation_;
			}

			void setJID(const JID& jid) {
				jid_ = jid;
			}

			const JID& getJID() const {
				return jid_;
			}

			void setPassword(const std::string& password) {
				password_ = password;
			}

			const std::string& getPassword() const {
				return password_;
			}

			void setReason(const std::string& text) {
				reason_ = text;
			}

			const std::string& getReason() const {
				return reason_;
			}

			void setThread(const std::string& thread) {
				thread_ = thread;
			}

			const std::string& getThread() const {
				return thread_;
			}

		private:
			bool continuation_;
			JID jid_;
			std::string password_;
			std::string reason_;
			std::string thread_;
	};
}
