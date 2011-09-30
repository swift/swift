/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class ByteArrayReadBytestream : public ReadBytestream {
		public:
			ByteArrayReadBytestream(const std::vector<unsigned char>& data) : data(data), position(0), dataComplete(true) {
			}

			virtual boost::shared_ptr<ByteArray> read(size_t size) {
				size_t readSize = size;
				if (position + readSize > data.size()) {
					readSize = data.size() - position;
				}
				boost::shared_ptr<ByteArray> result = boost::make_shared<ByteArray>(data.begin() + position, data.begin() + position + readSize);

				onRead(*result);
				position += readSize;
				return result;
			}

			virtual bool isFinished() const {
				return position >= data.size() && dataComplete;
			}

			virtual void setDataComplete(bool b) {
				dataComplete = b;
			}

			void addData(const std::vector<unsigned char>& moreData) {
				append(data, moreData);
				onDataAvailable();
			}

		private:
			std::vector<unsigned char> data;
			size_t position;
			bool dataComplete;
	};
}
