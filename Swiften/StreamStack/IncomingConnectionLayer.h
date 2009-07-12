#pragma once

#include <boost/signal.hpp>

#include "Swiften/StreamStack/LowLayer.h"
#include "Swiften/Network/IncomingConnection.h"

namespace Swift {
	class IncomingConnectionLayer : public LowLayer {
		public:
			IncomingConnectionLayer(IncomingConnection* connection) : connection_(connection) {
				connection_->onDataRead.connect(onDataRead);
			}

			void writeData(const ByteArray& data) {
				connection_->write(data);
			}

		private:
			IncomingConnection* connection_;
	};
}
