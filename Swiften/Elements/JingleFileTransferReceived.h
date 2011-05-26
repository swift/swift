/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Elements/StreamInitiationFileInfo.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {

class JingleFileTransferReceived : public Payload {
	public:
		typedef boost::shared_ptr<JingleFileTransferReceived> ref;

		void setFileInfo(const StreamInitiationFileInfo& fileInfo) {
			this->fileInfo = fileInfo;
		}

		const StreamInitiationFileInfo& getFileInfo() const {
			return this->fileInfo;
		}
	private:
		StreamInitiationFileInfo fileInfo;

};

}
