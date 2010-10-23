/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class Body : public Payload {
		public:
			typedef boost::shared_ptr<Body> ref;

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
