/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <string>

namespace Swift {
	class SWIFTEN_API Status : public Payload {
		public:
			Status(const std::string& text = "") : text_(text) {
			}

			void setText(const std::string& text) {
				text_ = text;
			}

			const std::string& getText() const {
				return text_;
			}

		private:
			std::string text_;
	};
}
