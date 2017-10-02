/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API AuthFailure : public ToplevelElement {
        public:
            typedef std::shared_ptr<AuthFailure> ref;

            AuthFailure() {}
    };
}
