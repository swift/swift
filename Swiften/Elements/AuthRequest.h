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
	class AuthRequest : public Element {
		public:
			AuthRequest(const std::string& mechanism = "") : mechanism_(mechanism) {
			}

			AuthRequest(const std::string& mechanism, const ByteArray& message) : 
					mechanism_(mechanism), message_(message) {
			}

			AuthRequest(const std::string& mechanism, const boost::optional<ByteArray>& message) : 
					mechanism_(mechanism), message_(message) {
			}

			const boost::optional<ByteArray>& getMessage() const {
				return message_;
			}

			void setMessage(const ByteArray& message) {
				message_ = boost::optional<ByteArray>(message);
			}

			const std::string& getMechanism() const {
				return mechanism_;
			}

			void setMechanism(const std::string& mechanism) {
				mechanism_ = mechanism;
			}

		private:
			std::string mechanism_;
			boost::optional<ByteArray> message_;
	};
}
