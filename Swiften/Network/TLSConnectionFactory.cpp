/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/TLSConnectionFactory.h>

#include <boost/shared_ptr.hpp>

#include <Swiften/Network/TLSConnection.h>

namespace Swift {

TLSConnectionFactory::TLSConnectionFactory(TLSContextFactory* contextFactory, ConnectionFactory* connectionFactory) : contextFactory(contextFactory), connectionFactory(connectionFactory){

}

TLSConnectionFactory::~TLSConnectionFactory() {

}


boost::shared_ptr<Connection> TLSConnectionFactory::createConnection() {
	return boost::make_shared<TLSConnection>(connectionFactory->createConnection(), contextFactory);
}

}
