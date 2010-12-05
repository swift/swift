/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Parser/ElementParser.h>

namespace Swift {
	class String;
	class PayloadParserFactoryCollection;

	template<typename ElementType>
	class GenericElementParser : public ElementParser {
		public:
			GenericElementParser() {
				stanza_ = boost::make_shared<ElementType>();
			}

			virtual boost::shared_ptr<Element> getElement() const {
				return stanza_;
			}

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
