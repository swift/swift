#ifndef SWIFTEN_CONNECTIONLAYER_H
#define SWIFTEN_CONNECTIONLAYER_H

#include <boost/signal.hpp>

#include "Swiften/StreamStack/LowLayer.h"
#include "Swiften/Network/Connection.h"

namespace Swift {
	class ConnectionLayer : public LowLayer {
		public:
			ConnectionLayer(Connection* connection) : connection_(connection) {
				connection_->onDataRead.connect(onDataRead);
			}

			void writeData(const ByteArray& data) {
				connection_->write(data);
			}

		private:
			Connection* connection_;
	};
}

#endif
