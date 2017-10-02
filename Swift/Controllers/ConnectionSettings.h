/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

struct ConnectionSettings {
        enum Method {
            Automatic,
            Manual,
            BOSH
        };
        enum ProxyType {
            None,
            System,
            SOCKS5,
            HTTPConnect
        };

        Method method;
        struct {
                bool useManualServer;
                std::string manualServerHostname;
                int manualServerPort;
                ProxyType proxyType;
                bool useManualProxy;
                std::string manualProxyHostname;
                int manualProxyPort;
        } manualSettings;
        struct {
                std::string boshURI;
                bool useManualProxy;
                std::string manualProxyHostname;
                int manualProxyPort;
        } boshSettings;
};
