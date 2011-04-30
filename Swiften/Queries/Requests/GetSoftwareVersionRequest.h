/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <boost/smart_ptr/make_shared.hpp>


namespace Swift {
	class GetSoftwareVersionRequest : public GenericRequest<SoftwareVersion> {
		public:
			typedef boost::shared_ptr<GetSoftwareVersionRequest> ref;

			static ref create(const JID& recipient, IQRouter* router) {
				return ref(new GetSoftwareVersionRequest(recipient, router));
			}

		private:
			GetSoftwareVersionRequest(
					const JID& recipient, 
					IQRouter* router) :
						GenericRequest<SoftwareVersion>(
							IQ::Get, recipient, boost::make_shared<SoftwareVersion>(), router) {
			}
	};
}
