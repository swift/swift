/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Element.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class ElementSerializer {
		public:
			virtual ~ElementSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<Element> element) const = 0;
			virtual bool canSerialize(boost::shared_ptr<Element> element) const = 0;
	};
}
