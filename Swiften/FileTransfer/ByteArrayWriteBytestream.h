/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/FileTransfer/WriteBytestream.h>

namespace Swift {
	class ByteArrayWriteBytestream : public WriteBytestream {
		public:
			ByteArrayWriteBytestream() {
			}

			virtual void write(const std::vector<unsigned char>& bytes) {
				data.insert(data.end(), bytes.begin(), bytes.end());
				onWrite(bytes);
			}

			const std::vector<unsigned char>& getData() const {
				return data;
			}

		private:
			std::vector<unsigned char> data;
	};
}
