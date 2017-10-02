/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Element.h>

namespace Swift {
    class SWIFTEN_API Payload : public Element {
        public:
            typedef std::shared_ptr<Payload> ref;
        public:
            Payload() {}
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Payload)
            virtual ~Payload();

            SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(Payload)
    };
}
