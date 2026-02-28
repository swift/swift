/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class LowLayer;

    class SWIFTEN_API HighLayer {
            friend class StreamStack;

        public:
            HighLayer();
            virtual ~HighLayer();

            virtual void handleDataRead(const SafeByteArray& data) = 0;

        protected:
            LowLayer* getChildLayer() {
                return childLayer;
            }

            void setChildLayer(LowLayer* childLayer) {
                this->childLayer = childLayer;
            }

            void writeDataToChildLayer(const SafeByteArray& data);

        private:
            LowLayer* childLayer;
    };
}
