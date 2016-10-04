/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/SOCKS5ProxiedConnection.h>

#include <boost/bind.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>

using namespace Swift;

SOCKS5ProxiedConnection::SOCKS5ProxiedConnection(
        DomainNameResolver* resolver,
        ConnectionFactory* connectionFactory,
        TimerFactory* timerFactory,
        const std::string& proxyHost,
        int proxyPort) :
            ProxiedConnection(resolver, connectionFactory, timerFactory, proxyHost, proxyPort),
            proxyState_(Initial) {
            }

void SOCKS5ProxiedConnection::initializeProxy() {
    proxyState_ = ProxyAuthenticating;
    SafeByteArray socksConnect;
    socksConnect.push_back(0x05); // VER = SOCKS5 = 0x05
    socksConnect.push_back(0x01); // Number of authentication methods after this byte.
    socksConnect.push_back(0x00); // 0x00 == no authentication
    // buffer.push_back(0x01); // 0x01 == GSSAPI
    // buffer.push_back(0x02); // 0x02 ==  Username/Password
    // rest see RFC 1928 (http://tools.ietf.org/html/rfc1928)
    write(socksConnect);
}

void SOCKS5ProxiedConnection::handleProxyInitializeData(std::shared_ptr<SafeByteArray> data) {
    SafeByteArray socksConnect;
    boost::asio::ip::address rawAddress = getServer().getAddress().getRawAddress();
    assert(rawAddress.is_v4() || rawAddress.is_v6());

    if (proxyState_ == ProxyAuthenticating) {
        SWIFT_LOG(debug) << "ProxyAuthenticating response received, reply with the connect BYTEs" << std::endl;
        unsigned char choosenMethod = static_cast<unsigned char> ((*data)[1]);
        if ((*data)[0] == 0x05 && choosenMethod != 0xFF) {
            switch(choosenMethod) { // use the correct Method
                case 0x00:
                    try {
                        proxyState_ = ProxyConnecting;
                        socksConnect.push_back(0x05); // VER = SOCKS5 = 0x05
                        socksConnect.push_back(0x01); // Construct a TCP connection. (CMD)
                        socksConnect.push_back(0x00); // reserved.
                        socksConnect.push_back(rawAddress.is_v4() ? 0x01 : 0x04); // IPv4 == 0x01, Hostname == 0x02, IPv6 == 0x04. (ATYP)
                        size_t size = rawAddress.is_v4() ? rawAddress.to_v4().to_bytes().size() : rawAddress.to_v6().to_bytes().size();
                        for (size_t s = 0; s < size; s++) {
                            unsigned char uc;
                            if(rawAddress.is_v4()) {
                                uc = rawAddress.to_v4().to_bytes()[s]; // the address.
                            }
                            else {
                                uc = rawAddress.to_v6().to_bytes()[s]; // the address.
                            }
                            socksConnect.push_back(uc);

                        }
                        socksConnect.push_back(static_cast<unsigned char> ((getServer().getPort() >> 8) & 0xFF)); // highbyte of the port.
                        socksConnect.push_back(static_cast<unsigned char> (getServer().getPort() & 0xFF)); // lowbyte of the port.
                        write(socksConnect);
                        return;
                    }
                    catch(...) {
                        SWIFT_LOG(error) << "exception caught" << std::endl;
                    }
                    write(socksConnect);
                    break;
                default:
                    setProxyInitializeFinished(true);
                    break;
            }
            return;
        }
        setProxyInitializeFinished(false);
    }
    else if (proxyState_ == ProxyConnecting) {
        SWIFT_LOG(debug) << "Connect response received, check if successfully." << std::endl;
        SWIFT_LOG(debug) << "Errorbyte: 0x" << std::hex << static_cast<int> ((*data)[1]) << std::dec << std::endl;
        /*

        data.at(1) can be one of the following:
        0x00     succeeded
        0x01     general SOCKS server failure
        0x02     connection not allowed by ruleset
        0x03     Network unreachable
        0x04     Host unreachable
        0x05     Connection refused
        0x06     TTL expired
        0x07     Command not supported (CMD)
        0x08     Address type not supported (ATYP)
        0x09 bis 0xFF     unassigned
        */
        if ((*data)[0] == 0x05 && (*data)[1] == 0x0) {
            SWIFT_LOG(debug) << "Successfully connected the server via the proxy." << std::endl;
            setProxyInitializeFinished(true);
        }
        else {
            SWIFT_LOG(error) << "SOCKS Proxy returned an error: " << std::hex << (*data)[1] << std::endl;
            setProxyInitializeFinished(false);
        }
    }
}
