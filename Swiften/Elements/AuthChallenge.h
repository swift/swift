/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <vector>

#include <Swiften/Elements/Element.h>

namespace Swift {
	class AuthChallenge : public Element {
		public:
			AuthChallenge() {
			}

			AuthChallenge(const std::vector<unsigned char>& value) : value(value) {
			}

			const boost::optional< std::vector<unsigned char> >& getValue() const {
				return value;
			}

			void setValue(const std::vector<unsigned char>& value) {
				this->value = boost::optional<std::vector<unsigned char> >(value);
			}

		private:
			boost::optional< std::vector<unsigned char> > value;
	};
}
