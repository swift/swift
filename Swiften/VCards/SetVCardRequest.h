/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/VCard.h>


namespace Swift {
	class SetVCardRequest : public GenericRequest<VCard> {
		public:
			typedef boost::shared_ptr<SetVCardRequest> ref;

			static ref create(VCard::ref vcard, IQRouter* router) {
				return ref(new SetVCardRequest(vcard, router));
			}

		private:
			SetVCardRequest(VCard::ref vcard, IQRouter* router) : GenericRequest<VCard>(IQ::Set, JID(), vcard, router) {
			}
	};
}
