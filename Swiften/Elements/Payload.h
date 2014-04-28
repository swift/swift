/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
	class SWIFTEN_API Payload : public ToplevelElement {
		public:
			typedef boost::shared_ptr<Payload> ref;
		public:
			Payload() {}
			SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Payload)
			virtual ~Payload();

			SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(Payload)
	};
}
