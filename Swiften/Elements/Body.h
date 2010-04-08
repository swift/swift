/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_Body_H
#define SWIFTEN_Body_H

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class Body : public Payload {
		public:
			Body(const String& text = "") : text_(text) {
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

#endif
