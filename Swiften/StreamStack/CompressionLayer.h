/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/noncopyable.hpp>
#include "Swiften/Base/boost_bsignals.h"

#include "Swiften/Base/ByteArray.h"
#include "Swiften/StreamStack/StreamLayer.h"
#include "Swiften/Compress/ZLibException.h"
#include "Swiften/Compress/ZLibCompressor.h"
#include "Swiften/Compress/ZLibDecompressor.h"

namespace Swift {
	class ZLibCompressor;
	class ZLibDecompressor;

	class CompressionLayer : public StreamLayer, boost::noncopyable {
		public:
			CompressionLayer() {}

			virtual void writeData(const ByteArray& data) {
				try {
					writeDataToChildLayer(compressor_.process(data));
				}
				catch (const ZLibException& e) {
					onError();
				}
			}

			virtual void handleDataRead(const ByteArray& data) {
				try {
					writeDataToParentLayer(decompressor_.process(data));
				}
				catch (const ZLibException& e) {
					onError();
				}
			}

		public:
			boost::signal<void ()> onError;

		private:
			ZLibCompressor compressor_;
			ZLibDecompressor decompressor_;
	};
}
