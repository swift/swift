/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/TLSConnectionFactory.h>

#include <memory>

#include <Swiften/Network/TLSConnection.h>

namespace Swift {

TLSConnectionFactory::TLSConnectionFactory(TLSContextFactory* contextFactory, ConnectionFactory* connectionFactory, const TLSOptions& o) : contextFactory(contextFactory), connectionFactory(connectionFactory), options_(o) {

}

TLSConnectionFactory::~TLSConnectionFactory() {

}


std::shared_ptr<Connection> TLSConnectionFactory::createConnection() {
    return std::make_shared<TLSConnection>(connectionFactory->createConnection(), contextFactory, options_);
}

}
