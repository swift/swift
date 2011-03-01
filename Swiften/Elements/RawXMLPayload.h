/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class RawXMLPayload : public Payload {
		public:
			RawXMLPayload(const std::string& data = "") : rawXML_(data) {}

			void setRawXML(const std::string& data) {
				rawXML_ = data;
			}

			const std::string& getRawXML() const {
				return rawXML_;
			}

		private:
			std::string rawXML_;
	};
}
