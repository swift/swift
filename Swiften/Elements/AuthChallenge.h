#pragma once

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/Element.h"

namespace Swift {
	class AuthChallenge : public Element {
		public:
			AuthChallenge(const ByteArray& value = "") : value(value) {
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
