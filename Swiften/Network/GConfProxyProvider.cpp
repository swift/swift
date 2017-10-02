/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/GConfProxyProvider.h>

#include <cstdio>
#include <cstdlib>

#include <iostream>

extern "C" {
#include <gconf/gconf-client.h>
}

#include <Swiften/Base/Log.h>

namespace Swift {

GConfProxyProvider::GConfProxyProvider() {
#if !GLIB_CHECK_VERSION(2,35,0)
    // Ensure static GLib initialization methods are called
    static bool glibInitialized = false;
    if (!glibInitialized) {
        g_type_init();
        glibInitialized = true;
    }
#endif

    socksProxy = getFromGConf("/system/proxy/socks_host", "/system/proxy/socks_port");
    httpProxy = getFromGConf("/system/http_proxy/host", "/system/http_proxy/port");
    SWIFT_LOG(debug) << "GConf: SOCKS5 => " << socksProxy.toString() << "; HTTP Connect => " << httpProxy.toString() << std::endl;
}

HostAddressPort GConfProxyProvider::getHTTPConnectProxy() const {
    return httpProxy;
}

HostAddressPort GConfProxyProvider::getSOCKS5Proxy() const {
    return socksProxy;
}

HostAddressPort GConfProxyProvider::getFromGConf(const char* gcHost, const char* gcPort) {
    std::string address;
    int port = 0;
    gchar* str;

    GConfClient* client = gconf_client_get_default();

    str = gconf_client_get_string(client, gcHost, NULL);
    port = static_cast<int> (gconf_client_get_int(client, gcPort, NULL));

    if(str) {
        address = static_cast<char*> (str);
        g_free(str);
    }

    g_object_unref(client);
    return HostAddressPort(HostAddress::fromString(address).get_value_or(HostAddress()), port);
}

}
