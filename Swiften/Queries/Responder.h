/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Queries/IQHandler.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
	/**
	 * A class for handling incoming IQ Get and Set requests of a specific payload type.
	 *
	 * Concrete subclasses of this class need to implement handleGetRequest() and handleSetRequest() to
	 * implement the behavior of the responder.
	 *
	 * \tparam PAYLOAD_TYPE The type of payload this Responder handles. Only IQ requests containing this
	 *	payload type will be passed to handleGetRequest() and handleSetRequest()
	 */
	template<typename PAYLOAD_TYPE>
	class Responder : public IQHandler {
		public:
			Responder(IQRouter* router) : router_(router) {
			}

			~Responder() {
			}

			/**
			 * Starts the responder.
			 *
			 * After the responder has started, it will start receiving and responding to requests.
			 *
			 * \see stop()
			 */
			void start() {
				router_->addHandler(this);
			}

			/**
			 * Stops the responder.
			 *
			 * When the responder is stopped, it will no longer receive incoming requests.
			 *
			 * \see start()
			 */
			void stop() {
				router_->removeHandler(this);
			}

		protected:
			/**
			 * Handle an incoming IQ-Get request containing a payload of class PAYLOAD_TYPE.
			 *
			 * This method is implemented in the concrete subclasses.
			 */
			virtual bool handleGetRequest(const JID& from, const JID& to, const std::string& id, boost::shared_ptr<PAYLOAD_TYPE> payload) = 0;

			/**
			 * Handle an incoming IQ-Set request containing a payload of class PAYLOAD_TYPE.
			 *
			 * This method is implemented in the concrete subclasses.
			 */
			virtual bool handleSetRequest(const JID& from, const JID& to, const std::string& id, boost::shared_ptr<PAYLOAD_TYPE> payload) = 0;

			/**
			 * Convenience function for sending an IQ response.
			 */
			void sendResponse(const JID& to, const std::string& id, boost::shared_ptr<PAYLOAD_TYPE> payload) {
				router_->sendIQ(IQ::createResult(to, id, payload));
			}

			/**
			 * Convenience function for sending an IQ response, with a specific from address.
			 */
			void sendResponse(const JID& to, const JID& from, const std::string& id, boost::shared_ptr<PAYLOAD_TYPE> payload) {
				router_->sendIQ(IQ::createResult(to, from, id, payload));
			}

			/**
			 * Convenience function for responding with an error.
			 */
			void sendError(const JID& to, const std::string& id, ErrorPayload::Condition condition, ErrorPayload::Type type) {
				router_->sendIQ(IQ::createError(to, id, condition, type));
			}

			/**
			 * Convenience function for responding with an error from a specific from address.
			 */
			void sendError(const JID& to, const JID& from, const std::string& id, ErrorPayload::Condition condition, ErrorPayload::Type type) {
				router_->sendIQ(IQ::createError(to, from, id, condition, type));
			}

			IQRouter* getIQRouter() const {
				return router_;
			}

		private:
			virtual bool handleIQ(boost::shared_ptr<IQ> iq) {
				if (iq->getType() == IQ::Set || iq->getType() == IQ::Get) {
					boost::shared_ptr<PAYLOAD_TYPE> payload(iq->getPayload<PAYLOAD_TYPE>());
					if (payload) {
						bool result;
						if (iq->getType() == IQ::Set) {
							result = handleSetRequest(iq->getFrom(), iq->getTo(), iq->getID(), payload);
						}
						else {
							result = handleGetRequest(iq->getFrom(), iq->getTo(), iq->getID(), payload);
						}
						if (!result) {
							router_->sendIQ(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::NotAllowed, ErrorPayload::Cancel));
						}
						return true;
					}
				}
				return false;
			}
		
		private:
			IQRouter* router_;
	};
}
