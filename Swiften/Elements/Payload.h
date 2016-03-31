/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Element.h>

namespace Swift {
    class SWIFTEN_API Payload : public Element {
        public:
            typedef boost::shared_ptr<Payload> ref;
        public:
            Payload() {}
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Payload)
            virtual ~Payload();

            SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(Payload)
    };
}
