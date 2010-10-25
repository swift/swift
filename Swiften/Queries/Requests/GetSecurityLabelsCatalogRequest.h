/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Elements/SecurityLabelsCatalog.h"


namespace Swift {
	class GetSecurityLabelsCatalogRequest : public GenericRequest<SecurityLabelsCatalog> {
		public:
			typedef boost::shared_ptr<GetSecurityLabelsCatalogRequest> ref;

			static ref create(const JID& recipient, IQRouter* router) {
				return ref(new GetSecurityLabelsCatalogRequest(recipient, router));
			}

		private:
			GetSecurityLabelsCatalogRequest(
					const JID& recipient, 
					IQRouter* router) :
						GenericRequest<SecurityLabelsCatalog>(
							IQ::Get, JID(), boost::shared_ptr<SecurityLabelsCatalog>(new SecurityLabelsCatalog(recipient)), router) {
			}
	};
}
