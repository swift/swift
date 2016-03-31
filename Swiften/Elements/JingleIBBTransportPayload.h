/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleTransportPayload.h>

namespace Swift {
    class SWIFTEN_API JingleIBBTransportPayload : public JingleTransportPayload {
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
