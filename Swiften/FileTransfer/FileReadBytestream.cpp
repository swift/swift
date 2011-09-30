/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/filesystem/fstream.hpp>
#include <cassert>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/FileTransfer/FileReadBytestream.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {

FileReadBytestream::FileReadBytestream(const boost::filesystem::path& file) : file(file), stream(NULL) {
}

FileReadBytestream::~FileReadBytestream() {
	if (stream) {
		stream->close();
		stream = NULL;
	}
}

boost::shared_ptr<ByteArray> FileReadBytestream::read(size_t size)  {
	if (!stream) {
		stream = new boost::filesystem::ifstream(file, std::ios_base::in|std::ios_base::binary);
	}
	boost::shared_ptr<ByteArray> result = boost::make_shared<ByteArray>();
	result->resize(size);
	assert(stream->good());
	stream->read(reinterpret_cast<char*>(vecptr(*result)), size);
	result->resize(stream->gcount());
	onRead(*result);
	return result;
}

bool FileReadBytestream::isFinished() const {
	return stream && !stream->good();
}

}
