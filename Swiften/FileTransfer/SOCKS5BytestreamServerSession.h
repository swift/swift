/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/WriteBytestream.h>
#include <Swiften/FileTransfer/FileTransferError.h>

namespace Swift {
	class SOCKS5BytestreamRegistry;

	class SOCKS5BytestreamServerSession {
		public:
		typedef boost::shared_ptr<SOCKS5BytestreamServerSession> ref;

		public:
			enum State {
				Initial,
				WaitingForAuthentication,
				WaitingForRequest,
				ReadyForTransfer,
				ReadingData,
				WritingData,
				Finished,
			};

			SOCKS5BytestreamServerSession(boost::shared_ptr<Connection> connection, SOCKS5BytestreamRegistry* registry);
			~SOCKS5BytestreamServerSession();

			void setChunkSize(int chunkSize) {
				this->chunkSize = chunkSize;
			}

			void start();
			void stop();

			void startTransfer();
			HostAddressPort getAddressPort() const;

			boost::signal<void (boost::optional<FileTransferError>)> onFinished;
			boost::signal<void (int)> onBytesSent;
			boost::signal<void (int)> onBytesReceived;

		private:
			void finish(bool error);
			void process();
			void handleDataRead(boost::shared_ptr<SafeByteArray>);
			void handleDisconnected(const boost::optional<Connection::Error>&);
			void sendData();

		private:
			boost::shared_ptr<Connection> connection;
			SOCKS5BytestreamRegistry* bytestreams;
			ByteArray unprocessedData;
			State state;
			int chunkSize;
			boost::shared_ptr<ReadBytestream> readBytestream;
			boost::shared_ptr<WriteBytestream> writeBytestream;
	};
}
