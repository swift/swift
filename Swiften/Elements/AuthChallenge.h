/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/Element.h"

namespace Swift {
	class AuthChallenge : public Element {
		public:
			AuthChallenge() {
			}

			AuthChallenge(const ByteArray& value) : value(value) {
			}

			const boost::optional<ByteArray>& getValue() const {
				return value;
			}

			void setValue(const ByteArray& value) {
				this->value = boost::optional<ByteArray>(value);
			}

		private:
			boost::optional<ByteArray> value;
	};
}
