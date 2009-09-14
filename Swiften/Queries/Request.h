#ifndef SWIFTEN_Request_H
#define SWIFTEN_Request_H

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Queries/IQHandler.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/Payload.h"
#include "Swiften/Elements/Error.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class Request : public IQHandler, public boost::enable_shared_from_this<Request> {
		public:
			Request(
					IQ::Type type, 
					const JID& receiver, 
					boost::shared_ptr<Payload> payload, 
					IQRouter* router);
			Request(
					IQ::Type type, 
					const JID& receiver, 
					IQRouter* router);

			void send();

		protected:
			virtual void setPayload(boost::shared_ptr<Payload> p) {
				payload_ = p;
			}

			virtual void handleResponse(boost::shared_ptr<Payload>, boost::optional<Error>) = 0;

		private:
			bool handleIQ(boost::shared_ptr<IQ>);

		private:
			IQRouter* router_;
			IQ::Type type_;
			JID receiver_;
			boost::shared_ptr<Payload> payload_;
			String id_;
			bool sent_;
	};
}

#endif
