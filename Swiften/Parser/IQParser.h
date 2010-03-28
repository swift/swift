#ifndef SWIFTEN_IQParser_H
#define SWIFTEN_IQParser_H

#include "Swiften/Parser/GenericStanzaParser.h"
#include "Swiften/Elements/IQ.h"

namespace Swift {
	class IQParser : public GenericStanzaParser<IQ> {
		public:
			IQParser(PayloadParserFactoryCollection* factories);

		private:
			virtual void handleStanzaAttributes(const AttributeMap&);
	};
}

#endif
