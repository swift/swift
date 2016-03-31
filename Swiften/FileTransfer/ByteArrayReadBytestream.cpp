/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/ByteArrayReadBytestream.h>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Algorithm.h>

using namespace Swift;

boost::shared_ptr<ByteArray> ByteArrayReadBytestream::read(size_t size) {
    size_t readSize = size;
    if (position + readSize > data.size()) {
        readSize = data.size() - position;
    }
    boost::shared_ptr<ByteArray> result = boost::make_shared<ByteArray>(
            data.begin() + boost::numeric_cast<long long>(position),
            data.begin() + boost::numeric_cast<long long>(position) + boost::numeric_cast<long long>(readSize));

    onRead(*result);
    position += readSize;
    return result;
}

void ByteArrayReadBytestream::addData(const std::vector<unsigned char>& moreData) {
    append(data, moreData);
    onDataAvailable();
}
