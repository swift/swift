/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class VCardUpdate : public Payload {
		public:
			VCardUpdate(const String& photoHash = "") : photoHash_(photoHash) {}

			void setPhotoHash(const String& photoHash) { photoHash_ = photoHash; }
			const String& getPhotoHash() { return photoHash_; }

		private:
			String photoHash_;
	};
}
