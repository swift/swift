/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

#pragma once

#include <Swiften/Elements/DeliveryReceipt.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class DeliveryReceiptParser : public GenericPayloadParser<DeliveryReceipt> {
		public:
			DeliveryReceiptParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributeMap);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			int level_;
	};
}
