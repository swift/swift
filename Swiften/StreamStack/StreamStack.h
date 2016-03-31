/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/Stanza.h>

namespace Swift {
    class XMPPLayer;
    class LowLayer;
    class StreamLayer;

    class SWIFTEN_API StreamStack {
        public:
            StreamStack(XMPPLayer* xmppLayer, LowLayer* physicalLayer);
            ~StreamStack();

            void addLayer(StreamLayer*);

            XMPPLayer* getXMPPLayer() const {
                return xmppLayer_;
            }

            template<typename T> T* getLayer() {
                for (size_t i = 0; i < layers_.size(); ++i) {
                    T* layer = dynamic_cast<T*>(layers_[i]);
                    if (layer) {
                        return layer;
                    }
                }
                return NULL;
            }

        private:
            XMPPLayer* xmppLayer_;
            LowLayer* physicalLayer_;
            std::vector<StreamLayer*> layers_;
    };
}
