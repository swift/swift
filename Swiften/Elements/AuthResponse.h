/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>

#include <Swiften/Elements/Element.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class AuthResponse : public Element {
		public:
			AuthResponse() {
			}

			AuthResponse(const SafeByteArray& value) : value(value) {
			}

			AuthResponse(const boost::optional<SafeByteArray>& value) : value(value) {
			}

			const boost::optional<SafeByteArray>& getValue() const {
				return value;
			}

			void setValue(const SafeByteArray& value) {
				this->value = boost::optional<SafeByteArray>(value);
			}

		private:
			boost::optional<SafeByteArray> value;
	};
}
