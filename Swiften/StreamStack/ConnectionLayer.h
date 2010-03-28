#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/StreamStack/LowLayer.h"
#include "Swiften/Network/Connection.h"

namespace Swift {
	class ConnectionLayer : public LowLayer {
		public:
			ConnectionLayer(boost::shared_ptr<Connection> connection) : connection(connection) {
				connection->onDataRead.connect(onDataRead);
			}

			void writeData(const ByteArray& data) {
				connection->write(data);
			}

		private:
			boost::shared_ptr<Connection> connection;
	};
}
