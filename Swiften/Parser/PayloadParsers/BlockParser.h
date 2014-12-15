/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Nickname.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	template<typename BLOCK_ELEMENT>
	class BlockParser : public GenericPayloadParser<BLOCK_ELEMENT> {
		public:
			BlockParser() : GenericPayloadParser<BLOCK_ELEMENT>(), level(0) {
			}

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
				if (level == 1 && element == "item") {
					JID jid(attributes.getAttribute("jid"));
					if (jid.isValid()) {
						GenericPayloadParser<BLOCK_ELEMENT>::getPayloadInternal()->addItem(jid);
					}
				}
				++level;
			}

			virtual void handleEndElement(const std::string&, const std::string&) {
				--level;
			}

			virtual void handleCharacterData(const std::string&) {
			}

		private:
			int level;
	};
}
