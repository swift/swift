/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>
#include <boost/optional.hpp>

#include <Swiften/Elements/Element.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class AuthRequest : public Element {
		public:
			AuthRequest(const std::string& mechanism = "") : mechanism_(mechanism) {
			}

			AuthRequest(const std::string& mechanism, const SafeByteArray& message) :
					mechanism_(mechanism), message_(message) {
			}

			AuthRequest(const std::string& mechanism, const boost::optional<SafeByteArray>& message) :
					mechanism_(mechanism), message_(message) {
			}

			const boost::optional<SafeByteArray>& getMessage() const {
				return message_;
			}

			void setMessage(const SafeByteArray& message) {
				message_ = boost::optional<SafeByteArray>(message);
			}

			const std::string& getMechanism() const {
				return mechanism_;
			}

			void setMechanism(const std::string& mechanism) {
				mechanism_ = mechanism;
			}

		private:
			std::string mechanism_;
			boost::optional<SafeByteArray> message_;
	};
}
