/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class HighLayer;

    class SWIFTEN_API LowLayer {
            friend class StreamStack;
            friend class DummyStreamLayer;

        public:
            LowLayer();
            virtual ~LowLayer();

            virtual void writeData(const SafeByteArray& data) = 0;

        protected:
            HighLayer* getParentLayer() {
                return parentLayer;
            }

            void setParentLayer(HighLayer* parentLayer) {
                this->parentLayer = parentLayer;
            }

            void writeDataToParentLayer(const SafeByteArray& data);

        private:
            HighLayer* parentLayer;
    };
}
