/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class Body : public Payload {
		public:
			Body(const std::string& text = "") : text_(text) {
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
