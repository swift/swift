/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Element.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class AuthSuccess : public Element {
		public:
			AuthSuccess() {}

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
