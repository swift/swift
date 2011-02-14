/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Elements/JingleTransport.h>

namespace Swift {
	class JingleIBBTransport : public JingleTransport {
		public:
			enum StanzaType {
				IQStanza,
				MessageStanza,
			};

			void setStanzaType(StanzaType stanzaType) {
				this->stanzaType = stanzaType;
			}

			StanzaType getStanzaType() const {
				return stanzaType;
			}

			void setSessionID(const std::string& id) {
				sessionID = id;
			}

			const std::string& getSessionID() const {
				return sessionID;
			}

			int getBlockSize() const {
				return blockSize;
			}

			void setBlockSize(int blockSize) {
				this->blockSize = blockSize;
			}

		private:
			std::string sessionID;
			int blockSize;
			StanzaType stanzaType;
	};
}
