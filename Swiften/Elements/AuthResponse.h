/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/Element.h"

namespace Swift {
	class AuthResponse : public Element {
		public:
			AuthResponse(const ByteArray& value = "") : value(value) {
			}

			const ByteArray& getValue() const {
				return value;
			}

			void setValue(const ByteArray& value) {
				this->value = value;
			}

		private:
			ByteArray value;
	};
}
