/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/FileTransfer/FileTransferProgressInfo.h>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {

FileTransferProgressInfo::FileTransferProgressInfo(size_t completeBytes) : completeBytes(completeBytes), completedBytes(0), percentage(0) {
    onProgressPercentage(0);
}

void FileTransferProgressInfo::setBytesProcessed(size_t processedBytes) {
    int oldPercentage = int(double(completedBytes) / double(completeBytes) * 100.0);
    completedBytes += processedBytes;
    int newPercentage = int(double(completedBytes) / double(completeBytes) * 100.0);
    if (oldPercentage != newPercentage) {
        onProgressPercentage(newPercentage);
    }
    percentage = newPercentage;
}

int FileTransferProgressInfo::getPercentage() const {
    return percentage;
}

}
