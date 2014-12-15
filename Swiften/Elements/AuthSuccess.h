/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>

#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
	class AuthSuccess : public ToplevelElement {
		public:
			AuthSuccess() {}

			const boost::optional<std::vector<unsigned char> >& getValue() const {
				return value;
			}

			void setValue(const std::vector<unsigned char>& value) {
				this->value = boost::optional<std::vector<unsigned char> >(value);
			}

		private:
			boost::optional<std::vector<unsigned char> > value;
	};
}
