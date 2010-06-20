/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class Subject : public Payload {
		public:
			Subject(const String& text = "") : text_(text) {
			}

			void setText(const String& text) {
				text_ = text;
			}

			const String& getText() const {
				return text_;
			}

		private:
			String text_;
	};
}
