/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/cstdint.hpp>

namespace Swift {

class FileTransferProgressInfo {
public:
	FileTransferProgressInfo(boost::uintmax_t completeBytes);

public:
	void setBytesProcessed(int processedBytes);

	int getPercentage() const;
	boost::signal<void (int)> onProgressPercentage;

private:
	boost::uintmax_t completeBytes;
	boost::uintmax_t completedBytes;
	int percentage;
};

}
