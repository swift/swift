/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/filesystem/fstream.hpp>
#include <cassert>

#include "Swiften/FileTransfer/FileReadBytestream.h"

namespace Swift {

FileReadBytestream::FileReadBytestream(const boost::filesystem::path& file) : file(file), stream(NULL) {
}

FileReadBytestream::~FileReadBytestream() {
	if (stream) {
		stream->close();
		stream = NULL;
	}
}

ByteArray FileReadBytestream::read(size_t size)  {
	if (!stream) {
		stream = new boost::filesystem::ifstream(file, std::ios_base::in|std::ios_base::binary);
	}
	ByteArray result;
	result.resize(size);
	assert(stream->good());
	stream->read(reinterpret_cast<char*>(result.getData()), size);
	result.resize(stream->gcount());
	return result;
}

bool FileReadBytestream::isFinished() const {
	return stream && !stream->good();
}

}
