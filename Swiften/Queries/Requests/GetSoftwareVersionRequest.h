/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API GetSoftwareVersionRequest : public GenericRequest<SoftwareVersion> {
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
