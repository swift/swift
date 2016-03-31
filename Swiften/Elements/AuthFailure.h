/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API AuthFailure : public ToplevelElement {
        public:
            typedef boost::shared_ptr<AuthFailure> ref;

            AuthFailure() {}
    };
}
