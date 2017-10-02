/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FileWriteBytestream.h>

#include <cassert>

#include <boost/filesystem/fstream.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace Swift {

FileWriteBytestream::FileWriteBytestream(const boost::filesystem::path& file) : file(file), stream(nullptr) {
}

FileWriteBytestream::~FileWriteBytestream() {
    if (stream) {
        stream->close();
        delete stream;
        stream = nullptr;
    }
}

bool FileWriteBytestream::write(const std::vector<unsigned char>& data) {
    if (data.empty()) {
        return true;
    }
    if (!stream) {
        stream = new boost::filesystem::ofstream(file, std::ios_base::out|std::ios_base::binary);
    }
    if (stream->good()) {
        stream->write(reinterpret_cast<const char*>(&data[0]), boost::numeric_cast<std::streamsize>(data.size()));
        if (stream->good()) {
            onWrite(data);
            return true;
        }
    }
    return false;
}

void FileWriteBytestream::close() {
    if (stream) {
        stream->close();
        delete stream;
        stream = nullptr;
    }
}

}
