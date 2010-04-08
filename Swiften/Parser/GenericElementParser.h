/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_GenericElementParser_H
#define SWIFTEN_GenericElementParser_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Parser/ElementParser.h"

namespace Swift {
	class String;
	class PayloadParserFactoryCollection;

	template<typename ElementType>
	class GenericElementParser : public ElementParser {
		public:
			GenericElementParser() {
				stanza_ = boost::shared_ptr<ElementType>(new ElementType());
			}

			virtual boost::shared_ptr<Element> getElement() const {
				return stanza_;
			}

		protected:
			virtual boost::shared_ptr<ElementType> getElementGeneric() const {
				return stanza_;
			}

		private:
			virtual void handleStartElement(const String&, const String&, const AttributeMap&) {
			}

			virtual void handleEndElement(const String&, const String&) {
			}

			virtual void handleCharacterData(const String&) {
			}

		private:
			boost::shared_ptr<ElementType> stanza_;
	};
}

#endif
