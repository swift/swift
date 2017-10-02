/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <openssl/ssl.h>

#include <Swiften/TLS/Certificate.h>

namespace Swift {
    class OpenSSLCertificate : public Certificate {
        public:
            OpenSSLCertificate(std::shared_ptr<X509>);
            OpenSSLCertificate(const ByteArray& der);

            std::string getSubjectName() const {
                return subjectName;
            }

            std::vector<std::string> getCommonNames() const {
                return commonNames;
            }

            std::vector<std::string> getSRVNames() const {
                return srvNames;
            }

            std::vector<std::string> getDNSNames() const {
                return dnsNames;
            }

            std::vector<std::string> getXMPPAddresses() const {
                return xmppAddresses;
            }

            ByteArray toDER() const;

            std::shared_ptr<X509> getInternalX509() const {
                return cert;
            }

        private:
            void parse();

            void addSRVName(const std::string& name) {
                srvNames.push_back(name);
            }

            void addDNSName(const std::string& name) {
                dnsNames.push_back(name);
            }

            void addXMPPAddress(const std::string& addr) {
                xmppAddresses.push_back(addr);
            }

        private:
            std::shared_ptr<X509> cert;
            std::string subjectName;
            std::vector<std::string> commonNames;
            std::vector<std::string> dnsNames;
            std::vector<std::string> xmppAddresses;
            std::vector<std::string> srvNames;
    };
}
