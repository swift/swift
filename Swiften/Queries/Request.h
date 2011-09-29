/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <string>
#include <Swiften/Queries/IQHandler.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	/**
	 * An IQ get/set request query.
	 */
	class Request : public IQHandler, public boost::enable_shared_from_this<Request> {
		public:
			void send();

			const JID& getReceiver() const {
				return receiver_;
			}

		protected:
			/**
			 * Constructs a request of a certain type to a specific receiver, and attaches the given
			 * payload.
			 */
			Request(
					IQ::Type type, 
					const JID& receiver, 
					boost::shared_ptr<Payload> payload, 
					IQRouter* router);

			/**
			 * Constructs a request of a certain type to a specific receiver from a specific sender, and attaches the given
			 * payload.
			 */
			Request(
					IQ::Type type,
					const JID& sender,
					const JID& receiver,
					boost::shared_ptr<Payload> payload,
					IQRouter* router);


			/**
			 * Constructs a request of a certain type to a specific receiver.
			 */
			Request(
					IQ::Type type, 
					const JID& receiver, 
					IQRouter* router);

			/**
			 * Constructs a request of a certain type to a specific receiver from a specific sender.
			 */
			Request(
					IQ::Type type,
					const JID& sender,
					const JID& receiver,
					IQRouter* router);


			virtual void setPayload(boost::shared_ptr<Payload> payload) {
				payload_ = payload;
			}

			boost::shared_ptr<Payload> getPayload() const {
				return payload_;
			}

			virtual void handleResponse(boost::shared_ptr<Payload>, boost::shared_ptr<ErrorPayload>) = 0;

		private:
			bool handleIQ(boost::shared_ptr<IQ>);
			bool isCorrectSender(const JID& jid);

		private:
			IQRouter* router_;
			IQ::Type type_;
			JID sender_;
			JID receiver_;
			boost::shared_ptr<Payload> payload_;
			std::string id_;
			bool sent_;
	};
}
