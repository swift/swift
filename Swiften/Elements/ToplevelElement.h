/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Element.h>

namespace Swift {
    class SWIFTEN_API ToplevelElement : public Element {
        public:
            ToplevelElement() {}
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(ToplevelElement)
            virtual ~ToplevelElement();
    };
}
