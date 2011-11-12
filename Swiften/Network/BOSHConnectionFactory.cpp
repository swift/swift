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

#include <Swiften/Network/BOSHConnectionFactory.h>
#include <Swiften/Network/BOSHConnection.h>

namespace Swift {

BOSHConnectionFactory::BOSHConnectionFactory(const URL& boshURL, ConnectionFactory* connectionFactory, XMLParserFactory* xmlParserFactory, TLSContextFactory* tlsFactory) : boshURL(boshURL), connectionFactory(connectionFactory), xmlParserFactory(xmlParserFactory), tlsFactory(tlsFactory) {

}

boost::shared_ptr<Connection> BOSHConnectionFactory::createConnection(ConnectionFactory* overrideFactory) {
	return BOSHConnection::create(boshURL, overrideFactory != NULL ? overrideFactory : connectionFactory, xmlParserFactory, tlsFactory);
}

}
