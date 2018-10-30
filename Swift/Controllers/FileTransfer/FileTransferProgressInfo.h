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

#pragma once

#include <cstddef>

#include <boost/signals2.hpp>

namespace Swift {

class FileTransferProgressInfo {
public:
    FileTransferProgressInfo(size_t completeBytes);

public:
    void setBytesProcessed(size_t processedBytes);

    int getPercentage() const;
    boost::signals2::signal<void (int)> onProgressPercentage;

private:
    size_t completeBytes;
    size_t completedBytes;
    int percentage;
};

}
