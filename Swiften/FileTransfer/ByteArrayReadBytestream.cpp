/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/ByteArrayReadBytestream.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/Algorithm.h>

using namespace Swift;

std::shared_ptr<ByteArray> ByteArrayReadBytestream::read(size_t size) {
    size_t readSize = size;
    if (position + readSize > data.size()) {
        readSize = data.size() - position;
    }
    try {
        std::shared_ptr<ByteArray> result = std::make_shared<ByteArray>(
                data.begin() + boost::numeric_cast<long long>(position),
                data.begin() + boost::numeric_cast<long long>(position) + boost::numeric_cast<long long>(readSize));
        onRead(*result);
        position += readSize;
        return result;
    }
    catch (const boost::numeric::bad_numeric_cast&) {
        // If we cannot cast to long long, we probably ran out of memory long ago
        assert(false);
        return {};
    }
}

void ByteArrayReadBytestream::addData(const std::vector<unsigned char>& moreData) {
    append(data, moreData);
    onDataAvailable();
}
