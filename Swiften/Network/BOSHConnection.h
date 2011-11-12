/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#pragma once

#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Network/Connection.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/URL.h>
#include <Swiften/Base/Error.h>
#include <Swiften/Session/SessionStream.h>

namespace boost {
	class thread;
	namespace system {
		class error_code;
	}
}

namespace Swift {
	class ConnectionFactory;
	class XMLParserFactory;
	class TLSContextFactory;

		class BOSHError : public SessionStream::Error {
				public:
					enum Type {BadRequest, HostGone, HostUnknown, ImproperAddressing, 
						  InternalServerError, ItemNotFound, OtherRequest, PolicyViolation, 
						  RemoteConnectionFailed, RemoteStreamError, SeeOtherURI, SystemShutdown, UndefinedCondition,
						  NoError};
					BOSHError(Type type) : SessionStream::Error(SessionStream::Error::ConnectionReadError), type(type) {}
					Type getType() {return type;}
					typedef boost::shared_ptr<BOSHError> ref;
				private:
					Type type;
					
			};


	class BOSHConnection : public Connection, public boost::enable_shared_from_this<BOSHConnection> {
		public:
			typedef boost::shared_ptr<BOSHConnection> ref;
			static ref create(const URL& boshURL, ConnectionFactory* connectionFactory, XMLParserFactory* parserFactory, TLSContextFactory* tlsFactory) {
				return ref(new BOSHConnection(boshURL, connectionFactory, parserFactory, tlsFactory));
			}
			virtual ~BOSHConnection();
			virtual void listen();
			virtual void connect(const HostAddressPort& address);
			virtual void disconnect();
			virtual void write(const SafeByteArray& data);

			virtual HostAddressPort getLocalAddress() const;
			const std::string& getSID();
			void setRID(unsigned long rid);
			void setSID(const std::string& sid);
			void startStream(const std::string& to, unsigned long rid);
			void terminateStream();
			bool isReadyToSend();
			void restartStream();

			static std::pair<SafeByteArray, size_t> createHTTPRequest(const SafeByteArray& data, bool streamRestart, bool terminate, long rid, const std::string& sid, const URL& boshURL);

			boost::signal<void (BOSHError::ref)> onSessionTerminated;
			boost::signal<void (const std::string& /*sid*/, size_t /*requests*/)> onSessionStarted;
			boost::signal<void (const SafeByteArray&)> onXMPPDataRead;
			boost::signal<void (const SafeByteArray&)> onBOSHDataRead;
			boost::signal<void (const SafeByteArray&)> onBOSHDataWritten;
			boost::signal<void (const std::string&)> onHTTPError;
		private:
			BOSHConnection(const URL& boshURL, ConnectionFactory* connectionFactory, XMLParserFactory* parserFactory, TLSContextFactory* tlsFactory);


			void handleConnectionConnectFinished(bool error);
			void handleDataRead(boost::shared_ptr<SafeByteArray> data);
			void handleDisconnected(const boost::optional<Error>& error);
			void write(const SafeByteArray& data, bool streamRestart, bool terminate); /* FIXME: refactor */
			BOSHError::Type parseTerminationCondition(const std::string& text);

			URL boshURL_;
			ConnectionFactory* connectionFactory_;
			XMLParserFactory* parserFactory_;
			boost::shared_ptr<Connection> connection_;
			std::string sid_;
			bool waitingForStartResponse_;
			unsigned long rid_;
			SafeByteArray buffer_;
			bool pending_;
			TLSContextFactory* tlsFactory_;
			bool connectionReady_;
	};
}
