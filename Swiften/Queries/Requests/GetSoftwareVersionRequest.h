/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API GetSoftwareVersionRequest : public GenericRequest<SoftwareVersion> {
        public:
            typedef std::shared_ptr<GetSoftwareVersionRequest> ref;

            static ref create(const JID& recipient, IQRouter* router) {
                return ref(new GetSoftwareVersionRequest(recipient, router));
            }

        private:
            GetSoftwareVersionRequest(
                    const JID& recipient,
                    IQRouter* router) :
                        GenericRequest<SoftwareVersion>(
                            IQ::Get, recipient, std::make_shared<SoftwareVersion>(), router) {
            }
    };
}
