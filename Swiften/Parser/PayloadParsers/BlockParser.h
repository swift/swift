/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Nickname.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	template<typename BLOCK_ELEMENT>
	class BlockParser : public GenericPayloadParser<BLOCK_ELEMENT> {
		public:
			BlockParser() : GenericPayloadParser<BLOCK_ELEMENT>() {
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
