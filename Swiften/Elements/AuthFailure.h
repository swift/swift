/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Element.h>

namespace Swift {
	class AuthFailure : public Element {
		public:
			typedef boost::shared_ptr<AuthFailure> ref;

			AuthFailure() {}
	};
}
