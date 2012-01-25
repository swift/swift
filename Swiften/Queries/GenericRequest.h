/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Queries/Request.h>

namespace Swift {
	/**
	 * GenericRequest is used for managing the sending of, and handling of replies to, iq stanzas that do not have their own Request types.
	 *
	 * To create an iq stanza, call a constructor with the type of the iq that needs to be sent (either Set or Get), addressing information (clients should use the constructor that doesn't specify a sender), the payload that should be sent in the iq, and the IQRouter for the connection, obtained through the Client or CoreClient object.
	 * 
	 * Having created a GenericRequest, connect to the onResponse signal to be told when a response (either a result or an error) has been received by Swiften.
	 *
	 * To send the iq, then call send() - onResponse will be called when a reply is received.
	 */
	template<typename PAYLOAD_TYPE>
	class GenericRequest : public Request {
		public:
			/**
			 * Create a request suitable for client use.
			 * @param type Iq type - Get or Set.
			 * @param receiver JID to send request to.
			 * @param payload Payload to send in stanza.
			 * @param router IQRouter instance for current connection.
			 */
			GenericRequest(
					IQ::Type type, 
					const JID& receiver, 
					boost::shared_ptr<Payload> payload, 
					IQRouter* router) :
						Request(type, receiver, payload, router) {
			}

			/**
			 * Create a request suitable for component or server use. As a client, use the other constructor instead.
			 * @param type Iq type - Get or Set.
			 * @param sender JID to use in "from" of stanza.
			 * @param receiver JID to send request to.
			 * @param payload Payload to send in stanza.
			 * @param router IQRouter instance for current connection.
			 */
			GenericRequest(
					IQ::Type type,
					const JID& sender,
					const JID& receiver,
					boost::shared_ptr<Payload> payload,
					IQRouter* router) :
						Request(type, sender, receiver, payload, router) {
			}

			/**
			 * Internal method, do not use.
			 */
			virtual void handleResponse(boost::shared_ptr<Payload> payload, ErrorPayload::ref error) {
				onResponse(boost::dynamic_pointer_cast<PAYLOAD_TYPE>(payload), error);
			}

		protected:
			boost::shared_ptr<PAYLOAD_TYPE> getPayloadGeneric() const {
				return boost::dynamic_pointer_cast<PAYLOAD_TYPE>(getPayload());
			}

		public:
			/**
			 * Signal emitted when a reply to the iq has been received. Contains a payload if one was present, and an error if one was present.
			 */
			boost::signal<void (boost::shared_ptr<PAYLOAD_TYPE>, ErrorPayload::ref)> onResponse;
	};
}
