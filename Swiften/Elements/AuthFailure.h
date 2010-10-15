/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Element.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class AuthFailure : public Element, public Shared<AuthFailure> {
		public:
			AuthFailure() {}
	};
}
