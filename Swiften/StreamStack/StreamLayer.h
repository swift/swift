/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/StreamStack/HighLayer.h>
#include <Swiften/StreamStack/LowLayer.h>

namespace Swift {
    class SWIFTEN_API StreamLayer : public LowLayer, public HighLayer {
        public:
            StreamLayer() {}
    };
}
