/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/cstdint.hpp>
#include <boost/signals2.hpp>

namespace Swift {

class FileTransferProgressInfo {
public:
    FileTransferProgressInfo(boost::uintmax_t completeBytes);

public:
    void setBytesProcessed(int processedBytes);

    int getPercentage() const;
    boost::signals2::signal<void (int)> onProgressPercentage;

private:
    boost::uintmax_t completeBytes;
    boost::uintmax_t completedBytes;
    int percentage;
};

}
