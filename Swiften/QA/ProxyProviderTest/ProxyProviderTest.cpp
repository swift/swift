/*
 * Copyright (c) 2010 Thilo Cestonaro
 * Licensed under the BSD License.
 * See Documentation/Licenses/BSD.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>

#include <Swiften/Network/PlatformProxyProvider.h>

using namespace Swift;

int main(void)
{
    int ret = 0;
    HostAddressPort hap;

    std::cout << "constructing PlatfromProxyProvider instance ..." << std::endl;
    PlatformProxyProvider ppp;

    hap = ppp.getSOCKS5Proxy();
    std::cout << "SOCKS5 Proxy configured: " << hap.isValid() << std::endl;
    if(hap.isValid()) {
        std::cout << "SOCKS5 Proxy: " << hap.getAddress().toString() << ":" << hap.getPort() << std::endl;
    }

    hap = ppp.getHTTPConnectProxy();
    std::cout << "HTTPConnect Proxy configured: " << hap.isValid() << std::endl;
    if(hap.isValid()) {
        std::cout << "HTTPConnect Proxy: " << hap.getAddress().toString() << ":" << hap.getPort() << std::endl;
    }

    return ret;
}
