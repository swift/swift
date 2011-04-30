/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>
#include <string>

namespace Swift {
	class Subject : public Payload {
		public:
			Subject(const std::string& text = "") : text_(text) {
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
