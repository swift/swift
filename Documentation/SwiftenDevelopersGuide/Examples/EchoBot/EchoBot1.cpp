/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Swiften.h>

using namespace Swift;

int main(int, char**) {
    SimpleEventLoop eventLoop;
    BoostNetworkFactories networkFactories(&eventLoop);

    Client client("echobot@wonderland.lit", "mypass", &networkFactories);
    client.setAlwaysTrustCertificates();
    client.connect();

    eventLoop.run();

    return 0;
}
