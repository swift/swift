/*
 * Copyright (c) 2011-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <string>

#include <Swiften/Elements/JingleTransportPayload.h>

namespace Swift {
	class JingleIBBTransportPayload : public JingleTransportPayload {
		public:
			typedef boost::shared_ptr<JingleIBBTransportPayload> ref;

			enum StanzaType {
				IQStanza,
				MessageStanza
			};

			JingleIBBTransportPayload() : stanzaType(IQStanza) {
			}

			void setStanzaType(StanzaType stanzaType) {
				this->stanzaType = stanzaType;
			}

			StanzaType getStanzaType() const {
				return stanzaType;
			}

			boost::optional<unsigned int> getBlockSize() const {
				return blockSize;
			}

			void setBlockSize(unsigned int blockSize) {
				this->blockSize = blockSize;
			}

		private:
			boost::optional<unsigned int> blockSize;
			StanzaType stanzaType;
	};
}
