/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/StreamStack/LowLayer.h>

namespace Swift {
    class SWIFTEN_API ConnectionLayer : public LowLayer {
        public:
            ConnectionLayer(std::shared_ptr<Connection> connection);
            virtual ~ConnectionLayer();

            void writeData(const SafeByteArray& data) {
                connection->write(data);
            }

        private:
            void handleDataRead(std::shared_ptr<SafeByteArray>);

        private:
            std::shared_ptr<Connection> connection;
    };
}
