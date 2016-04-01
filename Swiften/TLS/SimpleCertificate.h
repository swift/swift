/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/TLS/Certificate.h>

namespace Swift {
    class SWIFTEN_API SimpleCertificate : public Certificate {
        public:
            typedef std::shared_ptr<SimpleCertificate> ref;

            void setSubjectName(const std::string& name) {
                subjectName = name;
            }

            std::string getSubjectName() const {
                return subjectName;
            }

            std::vector<std::string> getCommonNames() const {
                return commonNames;
            }

            void addCommonName(const std::string& name) {
                commonNames.push_back(name);
            }

            void addSRVName(const std::string& name) {
                srvNames.push_back(name);
            }

            void addDNSName(const std::string& name) {
                dnsNames.push_back(name);
            }

            void addXMPPAddress(const std::string& addr) {
                xmppAddresses.push_back(addr);
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

            ByteArray toDER() const {
                return der;
            }

            void setDER(const ByteArray& der) {
                this->der = der;
            }

        private:
            void parse();

        private:
            std::string subjectName;
            ByteArray der;
            std::vector<std::string> commonNames;
            std::vector<std::string> dnsNames;
            std::vector<std::string> xmppAddresses;
            std::vector<std::string> srvNames;
    };
}
