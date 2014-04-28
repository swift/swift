/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
	class AuthFailure : public ToplevelElement {
		public:
			typedef boost::shared_ptr<AuthFailure> ref;

			AuthFailure() {}
	};
}
