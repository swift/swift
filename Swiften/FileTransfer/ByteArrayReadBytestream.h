/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/FileTransfer/ReadBytestream.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class ByteArrayReadBytestream : public ReadBytestream {
		public:
			ByteArrayReadBytestream(const ByteArray& data) : data(data), position(0) {
			}

			virtual ByteArray read(size_t size) {
				size_t readSize = size;
				if (position + readSize > data.getSize()) {
					readSize = data.getSize() - position;
				}
				ByteArray result(data.getData() + position, readSize);
				position += readSize;
				return result;
			}

			virtual bool isFinished() const {
				return position >= data.getSize();
			}

		private:
			ByteArray data;
			size_t position;
	};
}
