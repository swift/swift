/*
 * Copyright (c) 2011-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/JingleFileTransferFileInfo.h>

namespace Swift {
	class JingleFileTransferDescription : public JingleDescription {
		public:
			typedef boost::shared_ptr<JingleFileTransferDescription> ref;

			void setFileInfo(const JingleFileTransferFileInfo& fileInfo) {
				fileInfo_ = fileInfo;
			}

			const JingleFileTransferFileInfo& getFileInfo() {
				return fileInfo_;
			}

		private:
			JingleFileTransferFileInfo fileInfo_;
	};
}
