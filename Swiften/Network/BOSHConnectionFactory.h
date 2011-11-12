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

#include <string>

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/TLS/TLSContextFactory.h>
#include <Swiften/Base/URL.h>

namespace Swift {

class XMLParserFactory;

class BOSHConnectionFactory {
	public:
		BOSHConnectionFactory(const URL& boshURL, ConnectionFactory* connectionFactory, XMLParserFactory* xmlParserFactory, TLSContextFactory* tlsFactory);

		/**
		 * @param overrideFactory If non-NULL, creates a connection over the given factory instead.
		 */
		boost::shared_ptr<Connection> createConnection(ConnectionFactory* overrideFactory);
		ConnectionFactory* getRawConnectionFactory() {return connectionFactory;}
		TLSContextFactory* getTLSContextFactory() {return tlsFactory;}
	private:
		URL boshURL;
		ConnectionFactory* connectionFactory;
		XMLParserFactory* xmlParserFactory;
		TLSContextFactory* tlsFactory;
};

}
