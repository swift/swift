/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/Elements/MUCItem.h>

namespace Swift {
	class MUCUserPayload : public Payload {
		public:
			typedef boost::shared_ptr<MUCUserPayload> ref;

			struct StatusCode {
				StatusCode() : code(0) {}
				StatusCode(int code) : code(code) {}
				int code;
			};

			//struct Password {

			//}

			// struct History {

			// }

			/**
			 * reason is optional.
			 * from and to are mutually exclusive.
			 * From is used for MUC sending to invited client. To is used sending to MUC from inviting client.
			 * from is the JID the MUC claims the invite is from.
			 * to is the JID to send the invite to.
			 */
			struct Invite {
				std::string reason;
				JID from;
				JID to;
			};

			MUCUserPayload() {
			}

			void addItem(const MUCItem& item) {items_.push_back(item);}
		
			void addStatusCode(StatusCode code) {statusCodes_.push_back(code);}

			const std::vector<MUCItem>& getItems() const {return items_;}

			const std::vector<StatusCode>& getStatusCodes() const {return statusCodes_;}

			boost::shared_ptr<Payload> getPayload() const {
				return payload_;
			}

			void setPayload(boost::shared_ptr<Payload> p) {
				payload_ = p;
			}

			const boost::optional<std::string>& getPassword() const {
				return password_;
			}

			void setPassword(const std::string& password) {
				password_ = password;
			}


			const boost::optional<Invite>& getInvite() const {
				return invite_;
			}

			void setInvite(const Invite& invite) {
				invite_ = invite;
			}


		private:
			std::vector<MUCItem> items_;
			std::vector<StatusCode> statusCodes_;
			boost::shared_ptr<Payload> payload_;
			boost::optional<std::string> password_;
			boost::optional<Invite> invite_;
	};
}
