#ifndef SWIFTEN_ELEMENTSERIALIZER_H
#define SWIFTEN_ELEMENTSERIALIZER_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Element.h"

namespace Swift {
	class ElementSerializer {
		public:
			virtual ~ElementSerializer();

			virtual String serialize(boost::shared_ptr<Element> element) const = 0;
			virtual bool canSerialize(boost::shared_ptr<Element> element) const = 0;
	};
}

#endif
