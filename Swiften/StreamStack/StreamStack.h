/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Stanza.h>

namespace Swift {
    class HighLayer;
    class LowLayer;
    class StreamLayer;

    class SWIFTEN_API StreamStack {
        public:
            StreamStack(std::unique_ptr<HighLayer> topLayer, std::unique_ptr<LowLayer> bottomLayer);
            ~StreamStack();

            void addLayer(std::unique_ptr<StreamLayer> /* streamLayer */);

            HighLayer* getTopLayer() const {
                return topLayer_.get();
            }

            template<typename T> T* getLayer() const {
                for (const auto& i : layers_) {
                    T* layer = dynamic_cast<T*>(i.get());
                    if (layer) {
                        return layer;
                    }
                }
                if (T* layer = dynamic_cast<T*>(topLayer_.get())) {
                    return layer;
                }
                if (T* layer = dynamic_cast<T*>(bottomLayer_.get())) {
                    return layer;
                }
                return nullptr;
            }

        private:
            std::unique_ptr<HighLayer> topLayer_;
            std::unique_ptr<LowLayer> bottomLayer_;
            std::vector<std::unique_ptr<StreamLayer>> layers_;
    };
}
