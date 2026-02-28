/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FileReadBytestream.h>

#include <cassert>
#include <memory>

#include <boost/filesystem/fstream.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/ByteArray.h>

namespace Swift {

FileReadBytestream::FileReadBytestream(const boost::filesystem::path& file) : file(file), stream(nullptr) {
}

FileReadBytestream::~FileReadBytestream() {
    if (stream) {
        stream->close();
        delete stream;
        stream = nullptr;
    }
}

std::shared_ptr<ByteArray> FileReadBytestream::read(size_t size)  {
    if (!stream) {
        stream = new boost::filesystem::ifstream(file, std::ios_base::in|std::ios_base::binary);
    }
    std::shared_ptr<ByteArray> result = std::make_shared<ByteArray>();
    result->resize(size);
    assert(stream->good());
    stream->read(reinterpret_cast<char*>(vecptr(*result)), boost::numeric_cast<std::streamsize>(size));
    result->resize(boost::numeric_cast<size_t>(stream->gcount()));
    onRead(*result);
    return result;
}

bool FileReadBytestream::isFinished() const {
    return stream && !stream->good();
}

}
