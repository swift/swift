/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
