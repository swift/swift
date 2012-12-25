/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
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

			~FileTransferOptions();

			FileTransferOptions& withInBandAllowed(bool b) {
				allowInBand = b;
				return *this;
			}

			bool isInBandAllowed() const {
				return allowInBand;
			}

		private:
			bool allowInBand;
	};
}
