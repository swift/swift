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
