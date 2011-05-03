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

namespace Swift {
	class AuthRequest : public Element {
		public:
			AuthRequest(const std::string& mechanism = "") : mechanism_(mechanism) {
			}

			AuthRequest(const std::string& mechanism, const std::vector<unsigned char>& message) : 
					mechanism_(mechanism), message_(message) {
			}

			AuthRequest(const std::string& mechanism, const boost::optional<std::vector<unsigned char> >& message) : 
					mechanism_(mechanism), message_(message) {
			}

			const boost::optional<std::vector<unsigned char> >& getMessage() const {
				return message_;
			}

			void setMessage(const std::vector<unsigned char>& message) {
				message_ = boost::optional<std::vector<unsigned char> >(message);
			}

			const std::string& getMechanism() const {
				return mechanism_;
			}

			void setMechanism(const std::string& mechanism) {
				mechanism_ = mechanism;
			}

		private:
			std::string mechanism_;
			boost::optional<std::vector<unsigned char> > message_;
	};
}
