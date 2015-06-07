/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/ElementParser.h>

namespace Swift {
	
	class PayloadParserFactoryCollection;

	template<typename ElementType>
	class SWIFTEN_API GenericElementParser : public ElementParser {
		public:
			GenericElementParser() {
				stanza_ = boost::make_shared<ElementType>();
			}

			virtual boost::shared_ptr<ToplevelElement> getElement() const {
				return stanza_;
			}

			virtual boost::shared_ptr<ElementType> getElementGeneric() const {
				return stanza_;
			}

		private:
			virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
			}

			virtual void handleEndElement(const std::string&, const std::string&) {
			}

			virtual void handleCharacterData(const std::string&) {
			}

		private:
			boost::shared_ptr<ElementType> stanza_;
	};
}
