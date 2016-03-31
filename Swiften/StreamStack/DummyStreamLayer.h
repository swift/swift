/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/StreamStack/HighLayer.h>
#include <Swiften/StreamStack/LowLayer.h>

namespace Swift {
    /**
     *  The \ref DummyStreamLayer can be used to use a \ref LowLayer on its own, without a functioning parent layer.
     *  The \ref DummyStreamLayer will serve as the parent layer to the \ref LowLayer and is called when the \ref LowLayer
     *  wants to write data to its parent layer.
     */
    class SWIFTEN_API DummyStreamLayer : public HighLayer {
        public:
            DummyStreamLayer(LowLayer* lowLayer) {
                setChildLayer(lowLayer);
                lowLayer->setParentLayer(this);
            }

            virtual void handleDataRead(const SafeByteArray& /* data */) {

            }
    };
}
