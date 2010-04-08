/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
