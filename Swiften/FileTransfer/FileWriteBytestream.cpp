/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/filesystem/fstream.hpp>
#include <cassert>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/FileTransfer/FileWriteBytestream.h>

namespace Swift {

FileWriteBytestream::FileWriteBytestream(const boost::filesystem::path& file) : file(file), stream(NULL) {
}

FileWriteBytestream::~FileWriteBytestream() {
	if (stream) {
		stream->close();
		stream = NULL;
	}
}

void FileWriteBytestream::write(const std::vector<unsigned char>& data) {
	if (data.empty()) {
		return;
	}
	if (!stream) {
		stream = new boost::filesystem::ofstream(file, std::ios_base::out|std::ios_base::binary);
	}
	assert(stream->good());
	stream->write(reinterpret_cast<const char*>(&data[0]), boost::numeric_cast<std::streamsize>(data.size()));
	onWrite(data);
}

void FileWriteBytestream::close() {
	if (stream) {
		stream->close();
		stream = NULL;
	}
}

}
