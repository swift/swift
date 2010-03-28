#ifndef SWIFTEN_GenericElementSerializer_H
#define SWIFTEN_GenericElementSerializer_H

#include "Swiften/Serializer/ElementSerializer.h"

namespace Swift {
	template<typename T>
	class GenericElementSerializer : public ElementSerializer {
		public:
			virtual String serialize(boost::shared_ptr<Element> element) const = 0;

			virtual bool canSerialize(boost::shared_ptr<Element> element) const {
				return dynamic_cast<T*>(element.get()) != 0;
			}
	};
}

#endif
