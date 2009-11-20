#pragma once

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/AuthResponse.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class AuthResponseParser : public GenericElementParser<AuthResponse> {
		public:
			AuthResponseParser();

			virtual void handleStartElement(const String&, const String& ns, const AttributeMap&);
			virtual void handleEndElement(const String&, const String& ns);
			virtual void handleCharacterData(const String&);

		private:
			int depth;
			String text;
	};
}
