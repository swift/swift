/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/FileTransfer/ReadBytestream.h"

namespace Swift {
	class SOCKS5BytestreamRegistry;

	class SOCKS5BytestreamServerSession {
		public:
			enum State {
				Initial,
				WaitingForAuthentication,
				WaitingForRequest,
				SendingData,
				Finished,
			};

			SOCKS5BytestreamServerSession(boost::shared_ptr<Connection> connection, SOCKS5BytestreamRegistry* registry);
			~SOCKS5BytestreamServerSession();

			void setChunkSize(int chunkSize) {
				this->chunkSize = chunkSize;
			}

			void start();
			void stop();

			boost::signal<void (bool /* error */)> onFinished;

		private:
			void finish(bool error);
			void process();
			void handleDataRead(const ByteArray&);
			void sendData();

		private:
			boost::shared_ptr<Connection> connection;
			SOCKS5BytestreamRegistry* bytestreams;
			ByteArray unprocessedData;
			State state;
			int chunkSize;
			boost::shared_ptr<ReadBytestream> bytestream;
	};
}
