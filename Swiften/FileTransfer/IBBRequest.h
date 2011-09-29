/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/IBB.h>


namespace Swift {
	class IBBRequest : public GenericRequest<IBB> {
		public:
			typedef boost::shared_ptr<IBBRequest> ref;

			static ref create(const JID& from, const JID& to, boost::shared_ptr<IBB> payload, IQRouter* router) {
				return ref(new IBBRequest(from, to, payload, router));
			}

		private:
			IBBRequest(const JID& from, const JID& to, boost::shared_ptr<IBB> payload, IQRouter* router) : GenericRequest<IBB>(IQ::Set, from, to, payload, router) {
			}
	};
}
