#ifndef SWIFTEN_GetDiscoInfoRequest_H
#define SWIFTEN_GetDiscoInfoRequest_H

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/DiscoInfo.h"

namespace Swift {
	class GetDiscoInfoRequest : public GenericRequest<DiscoInfo> {
		public:
			GetDiscoInfoRequest(const JID& jid, IQRouter* router, AutoDeleteBehavior autoDeleteBehavior = DoNotAutoDelete) : 
					GenericRequest<DiscoInfo>(IQ::Get, jid, boost::shared_ptr<DiscoInfo>(new DiscoInfo()), router, autoDeleteBehavior) {
			}
	};
}

#endif
