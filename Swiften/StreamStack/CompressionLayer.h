/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Compress/ZLibCompressor.h>
#include <Swiften/Compress/ZLibDecompressor.h>
#include <Swiften/Compress/ZLibException.h>
#include <Swiften/StreamStack/StreamLayer.h>

namespace Swift {
    class ZLibCompressor;
    class ZLibDecompressor;

    class SWIFTEN_API CompressionLayer : public StreamLayer, boost::noncopyable {
        public:
            CompressionLayer() {}

            virtual void writeData(const SafeByteArray& data) {
                try {
                    writeDataToChildLayer(compressor_.process(data));
                }
                catch (const ZLibException&) {
                    onError();
                }
            }

            virtual void handleDataRead(const SafeByteArray& data) {
                try {
                    writeDataToParentLayer(decompressor_.process(data));
                }
                catch (const ZLibException&) {
                    onError();
                }
            }

        public:
            boost::signals2::signal<void ()> onError;

        private:
            ZLibCompressor compressor_;
            ZLibDecompressor decompressor_;
    };
}
