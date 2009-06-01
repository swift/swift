#ifndef SWIFTEN_Responder_H
#define SWIFTEN_Responder_H

#include "Swiften/Queries/IQHandler.h"
#include "Swiften/Elements/Error.h"

namespace Swift {
	template<typename PAYLOAD_TYPE>
	class Responder : public IQHandler {
		public:
			Responder(IQRouter* router) : IQHandler(router) {
			}

		protected:
			virtual bool handleGetRequest(const JID& from, const String& id, boost::shared_ptr<PAYLOAD_TYPE> payload) = 0;
			virtual bool handleSetRequest(const JID& from, const String& id, boost::shared_ptr<PAYLOAD_TYPE> payload) = 0;

			void sendResponse(const JID& to, const String& id, boost::shared_ptr<Payload> payload) {
				getRouter()->sendIQ(IQ::createResult(to, id, payload));
			}

			void sendError(const JID& to, const String& id, Error::Condition condition, Error::Type type) {
				getRouter()->sendIQ(IQ::createError(to, id, condition, type));
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
							getRouter()->sendIQ(IQ::createError(iq->getFrom(), iq->getID(), Error::NotAllowed, Error::Cancel));
						}
						return true;
					}
				}
				return false;
			}
	};
}

#endif
