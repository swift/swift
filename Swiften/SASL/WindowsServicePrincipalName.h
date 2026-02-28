/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Windows.h>

#include <Swiften/Base/API.h>

namespace Swift {
    /*
     * This represents the SPN used on Windows to identify a service
     * instance.
     */
    class SWIFTEN_API WindowsServicePrincipalName {
        public:
            /*
             * This assigns the provided service name,
             * "xmpp" service class, no instance name or referrer,
             * and default port. All of these (except the service
             * name) can be set to other values using the provided
             * methods.
             * If the constructor is called with the service name
             * "hostname.example.com" and the provided methods are
             * not used to change the defaults, then toString()
             * will return the SPN "xmpp/hostname.example.com".
             */
            WindowsServicePrincipalName(const std::string& serviceName);

            ~WindowsServicePrincipalName();

            void setServiceClass(const std::string& serviceClass);

            void setInstanceName(const std::string& instanceName);

            void setReferrer(const std::string& referrer);

            /*
             * This sets a non-default port for the service. Note
             * that the default value is 0 which indicates the
             * default port for the service. So if the XMPP service
             * is using the default port of 5222 for client
             * connections, then do not set the port to 5222 but let
             * it remain 0 to indicate that the default port is
             * used.
             */
            void setInstancePort(unsigned short instancePort) { instancePort_ = instancePort; }

            /*
             * This follows the rules of SPN creation on Windows and
             * returns the SPN string constructed from the set
             * values.
             */
            std::string toString();

        private:
            DWORD dsMakeSpn(DWORD* length, wchar_t* value);

        private:
            std::wstring serviceClass_;
            std::wstring serviceName_;
            std::wstring instanceName_;
            USHORT instancePort_;
            std::wstring referrer_;
    };
}
