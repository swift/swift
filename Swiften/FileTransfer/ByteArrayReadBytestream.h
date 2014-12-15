/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class ByteArrayReadBytestream : public ReadBytestream {
		public:
			ByteArrayReadBytestream(const std::vector<unsigned char>& data) : data(data), position(0), dataComplete(true) {
			}

			virtual boost::shared_ptr<ByteArray> read(size_t size);

			virtual bool isFinished() const {
				return position >= data.size() && dataComplete;
			}

			virtual void setDataComplete(bool b) {
				dataComplete = b;
			}

			void addData(const std::vector<unsigned char>& moreData);

		private:
			std::vector<unsigned char> data;
			size_t position;
			bool dataComplete;
	};
}
