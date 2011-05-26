/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/FileTransfer/ReadBytestream.h>

namespace Swift {
	class ByteArrayReadBytestream : public ReadBytestream {
		public:
			ByteArrayReadBytestream(const std::vector<unsigned char>& data) : data(data), position(0) {
			}

			virtual std::vector<unsigned char> read(size_t size) {
				size_t readSize = size;
				if (position + readSize > data.size()) {
					readSize = data.size() - position;
				}
				std::vector<unsigned char> result(data.begin() + position, data.begin() + position + readSize);

				onRead(result);
				position += readSize;
				return result;
			}

			virtual bool isFinished() const {
				return position >= data.size();
			}

		private:
			std::vector<unsigned char> data;
			size_t position;
	};
}
