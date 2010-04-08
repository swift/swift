/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_GenericStanzaParser_H
#define SWIFTEN_GenericStanzaParser_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Parser/StanzaParser.h"

namespace Swift {
	class String;
	class PayloadParserFactoryCollection;

	template<typename STANZA_TYPE>
	class GenericStanzaParser : public StanzaParser {
		public:
			GenericStanzaParser(PayloadParserFactoryCollection* collection) : 
						StanzaParser(collection) {
				stanza_ = boost::shared_ptr<STANZA_TYPE>(new STANZA_TYPE());
			}

			virtual boost::shared_ptr<Element> getElement() const {
				return stanza_;
			}

			virtual boost::shared_ptr<STANZA_TYPE> getStanzaGeneric() const {
				return stanza_;
			}

		private:
			boost::shared_ptr<STANZA_TYPE> stanza_;
	};
}

#endif
