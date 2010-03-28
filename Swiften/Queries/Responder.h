#ifndef SWIFTEN_Responder_H
#define SWIFTEN_Responder_H

#include "Swiften/Queries/IQHandler.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {
	template<typename PAYLOAD_TYPE>
	class Responder : public IQHandler {
		public:
			Responder(IQRouter* router) : router_(router) {
				router_->addHandler(this);
			}

			~Responder() {
				router_->removeHandler(this);
			}

		protected:
			virtual bool handleGetRequest(const JID& from, const String& id, boost::shared_ptr<PAYLOAD_TYPE> payload) = 0;
			virtual bool handleSetRequest(const JID& from, const String& id, boost::shared_ptr<PAYLOAD_TYPE> payload) = 0;

			void sendResponse(const JID& to, const String& id, boost::shared_ptr<Payload> payload) {
				router_->sendIQ(IQ::createResult(to, id, payload));
			}

			void sendError(const JID& to, const String& id, ErrorPayload::Condition condition, ErrorPayload::Type type) {
				router_->sendIQ(IQ::createError(to, id, condition, type));
			}

		private:
			virtual bool handleIQ(boost::shared_ptr<IQ> iq) {
				if (iq->getType() == IQ::Set || iq->getType() == IQ::Get) {
					boost::shared_ptr<PAYLOAD_TYPE> payload(iq->getPayload<PAYLOAD_TYPE>());
					if (payload) {
						bool result;
						if (iq->getType() == IQ::Set) {
							result = handleSetRequest(iq->getFrom(), iq->getID(), payload);
						}
						else {
							result = handleGetRequest(iq->getFrom(), iq->getID(), payload);
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

#endif
