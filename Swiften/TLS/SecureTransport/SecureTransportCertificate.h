/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/type_traits.hpp>

#include <Security/SecCertificate.h>

#include <Swiften/TLS/Certificate.h>

namespace Swift {

class SecureTransportCertificate : public Certificate {
public:
    SecureTransportCertificate(SecCertificateRef certificate);
    SecureTransportCertificate(const ByteArray& der);
    virtual ~SecureTransportCertificate();

    virtual std::string getSubjectName() const;
    virtual std::vector<std::string> getCommonNames() const;
    virtual std::vector<std::string> getSRVNames() const;
    virtual std::vector<std::string> getDNSNames() const;
    virtual std::vector<std::string> getXMPPAddresses() const;

    virtual ByteArray toDER() const;

private:
    void parse();
    typedef boost::remove_pointer<SecCertificateRef>::type SecCertificate;

private:
    std::shared_ptr<SecCertificate> certificateHandle_;
    std::string subjectName_;
    std::vector<std::string> commonNames_;
    std::vector<std::string> srvNames_;
    std::vector<std::string> dnsNames_;
    std::vector<std::string> xmppAddresses_;
};

}
