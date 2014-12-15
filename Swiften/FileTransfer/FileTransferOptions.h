/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API FileTransferOptions {
		public:
			FileTransferOptions() : allowInBand(false) {
			}
			SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(FileTransferOptions)
			~FileTransferOptions();

			FileTransferOptions& withInBandAllowed(bool b) {
				allowInBand = b;
				return *this;
			}

			bool isInBandAllowed() const {
				return allowInBand;
			}

			SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(FileTransferOptions)

		private:
			bool allowInBand;
	};
}
