#pragma once

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/SecurityLabelsCatalog.h"

namespace Swift {
	class GetSecurityLabelsCatalogRequest : public GenericRequest<SecurityLabelsCatalog> {
		public:
			GetSecurityLabelsCatalogRequest(
					const JID& recipient, 
					IQRouter* router) :
						GenericRequest<SecurityLabelsCatalog>(
							IQ::Get, JID(), boost::shared_ptr<SecurityLabelsCatalog>(new SecurityLabelsCatalog(recipient)), router) {
			}
	};
}
