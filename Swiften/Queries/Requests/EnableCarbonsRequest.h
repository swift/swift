/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CarbonsEnable.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class SWIFTEN_API EnableCarbonsRequest : public GenericRequest<CarbonsEnable> {
        public:
            typedef std::shared_ptr<EnableCarbonsRequest> ref;

            static ref create(IQRouter* router) {
                return ref(new EnableCarbonsRequest(router));
            }

        private:
            EnableCarbonsRequest(IQRouter* router) : GenericRequest<CarbonsEnable>(IQ::Set, JID(), std::make_shared<CarbonsEnable>(), router) {
            }
    };
}
