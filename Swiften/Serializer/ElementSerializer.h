/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ELEMENTSERIALIZER_H
#define SWIFTEN_ELEMENTSERIALIZER_H

#include <boost/shared_ptr.hpp>

#include <string>
#include "Swiften/Elements/Element.h"

namespace Swift {
	class ElementSerializer {
		public:
			virtual ~ElementSerializer();

			virtual std::string serialize(boost::shared_ptr<Element> element) const = 0;
			virtual bool canSerialize(boost::shared_ptr<Element> element) const = 0;
	};
}

#endif
