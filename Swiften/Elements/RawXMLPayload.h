#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class RawXMLPayload : public Payload {
		public:
			RawXMLPayload() {}

			void setRawXML(const String& data) {
				rawXML_ = data;
			}

			const String& getRawXML() const {
				return rawXML_;
			}

		private:
			String rawXML_;
	};
}
