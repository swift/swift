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
