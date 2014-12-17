/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/JingleFileTransferFileInfo.h>

namespace Swift {

class JingleFileTransferHash : public Payload {
public:
	typedef boost::shared_ptr<JingleFileTransferHash> ref;

	void setFileInfo(const JingleFileTransferFileInfo& fileInfo) {
		fileInfo_ = fileInfo;
	}

	JingleFileTransferFileInfo& getFileInfo() {
		return fileInfo_;
	}

private:
	JingleFileTransferFileInfo fileInfo_;
};
	
}
