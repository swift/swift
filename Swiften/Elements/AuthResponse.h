/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>

#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class AuthResponse : public ToplevelElement {
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
