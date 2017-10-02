/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API SetVCardRequest : public GenericRequest<VCard> {
        public:
            typedef std::shared_ptr<SetVCardRequest> ref;

            static ref create(VCard::ref vcard, IQRouter* router) {
                return ref(new SetVCardRequest(vcard, router));
            }

        private:
            SetVCardRequest(VCard::ref vcard, IQRouter* router) : GenericRequest<VCard>(IQ::Set, JID(), vcard, router) {
            }
    };
}
