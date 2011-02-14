/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>

#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/JingleContent.h>


namespace Swift {
	class JinglePayload : public Payload {
		public:
			typedef boost::shared_ptr<JinglePayload> ref;
			struct Reason {
					enum Type {
						AlternativeSession,
						Busy,
						Cancel,
						ConnectivityError,
						Decline,
						Expired,
						FailedApplication,
						FailedTransport,
						GeneralError,
						Gone,
						IncompatibleParameters,
						MediaError,
						SecurityError,
						Success,
						Timeout,
						UnsupportedApplications,
						UnsupportedTransports
					};
					
					Reason(Type type, const std::string& text = "") : type(type), text(text) {}
					Type type;
					std::string text;
			};

			enum Action {
				ContentAccept,
				ContentAdd,
				ContentModify,
				ContentReject,
				ContentRemove,
				DescriptionInfo,
				SecurityInfo,
				SessionAccept,
				SessionInfo,
				SessionInitiate,
				SessionTerminate,
				TransportAccept,
				TransportInfo,
				TransportReject,
				TransportReplace
			};

			JinglePayload(Action action, const std::string& sessionID) : action(action), sessionID(sessionID) {
			}

			void setAction(Action action) {
				this->action = action;
			}

			Action getAction() const {
				return action;
			}

			void setInitiator(const JID& initiator) {
				this->initiator = initiator;
			}

			const JID& getInitiator() const {
				return initiator;
			}

			void setResponder(const JID& responder) {
				this->responder = responder;
			}

			const JID& getResponder() const {
				return responder;
			}

			void setSessionID(const std::string& id) {
				sessionID = id;
			}

			const std::string& getSessionID() const {
				return sessionID;
			}

			void addContent(JingleContent::ref content) {
				this->contents.push_back(content);
			}

			const std::vector<JingleContent::ref> getContents() const {
				return contents;
			}

			void setReason(const Reason& reason) {
				this->reason = reason;
			}

			const boost::optional<Reason>& getReason() const {
				return reason;
			}

		private:
			Action action;
			JID initiator;
			JID responder;
			std::string sessionID;
			std::vector<JingleContent::ref> contents;
			boost::optional<Reason> reason;
	};
}
