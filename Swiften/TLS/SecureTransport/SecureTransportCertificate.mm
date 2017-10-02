/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/SecureTransport/SecureTransportCertificate.h>

#include <boost/numeric/conversion/cast.hpp>

#include <Cocoa/Cocoa.h>
#include <Security/Security.h>

#include <Swiften/Base/Log.h>

namespace {

template <typename T, typename S>
T bridge_cast(S source) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
    return (__bridge T)(source);
#pragma clang diagnostic pop
}

}

namespace {

inline std::string ns2StdString(NSString* _Nullable nsString);
inline std::string ns2StdString(NSString* _Nullable nsString) {
    std::string stdString;
    if (nsString != nil) {
        stdString = std::string([nsString cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    return stdString;
}

}

namespace Swift {

SecureTransportCertificate::SecureTransportCertificate(SecCertificateRef certificate) {
    assert(certificate);
    CFRetain(certificate);
    certificateHandle_ = std::shared_ptr<SecCertificate>(certificate, CFRelease);
    parse();
}


SecureTransportCertificate::SecureTransportCertificate(const ByteArray& der) {
    CFDataRef derData = CFDataCreateWithBytesNoCopy(nullptr, der.data(), static_cast<CFIndex>(der.size()), nullptr);
    // certificate will take ownership of derData and free it on its release.
    SecCertificateRef certificate = SecCertificateCreateWithData(nullptr, derData);
    if (certificate) {
        certificateHandle_ = std::shared_ptr<SecCertificate>(certificate, CFRelease);
        parse();
    }
}

SecureTransportCertificate::~SecureTransportCertificate() {

}

void SecureTransportCertificate::parse() {
    assert(certificateHandle_);
    CFErrorRef error = nullptr;

    // The SecCertificateCopyValues function is not part of the iOS Secure Transport API.
    CFDictionaryRef valueDict = SecCertificateCopyValues(certificateHandle_.get(), nullptr, &error);
    if (valueDict) {
        // Handle subject.
        CFStringRef subject = SecCertificateCopySubjectSummary(certificateHandle_.get());
        if (subject) {
            NSString* subjectStr = bridge_cast<NSString*>(subject);
            subjectName_ = ns2StdString(subjectStr);
            CFRelease(subject);
        }

        // Handle a single Common Name.
        CFStringRef commonName = nullptr;
        OSStatus error = SecCertificateCopyCommonName(certificateHandle_.get(), &commonName);
        if (!error && commonName) {
            NSString* commonNameStr = bridge_cast<NSString*>(commonName);
            commonNames_.push_back(ns2StdString(commonNameStr));
        }
        if (commonName) {
            CFRelease(commonName);
        }

        // Handle Subject Alternative Names
        NSDictionary* certDict = bridge_cast<NSDictionary*>(valueDict);
        NSDictionary* subjectAltNamesDict = certDict[@"2.5.29.17"][@"value"];

        for (NSDictionary* entry in subjectAltNamesDict) {
            NSString* label = entry[@"label"];
            if ([label isEqualToString:static_cast<NSString * _Nonnull>([NSString stringWithUTF8String:ID_ON_XMPPADDR_OID])]) {
                xmppAddresses_.push_back(ns2StdString(entry[@"value"]));
            }
            else if ([label isEqualToString:static_cast<NSString * _Nonnull>([NSString stringWithUTF8String:ID_ON_DNSSRV_OID])]) {
                srvNames_.push_back(ns2StdString(entry[@"value"]));
            }
            else if ([label isEqualToString:@"DNS Name"]) {
                dnsNames_.push_back(ns2StdString(entry[@"value"]));
            }
        }
        CFRelease(valueDict);
    }

    if (error) {
        CFRelease(error);
    }
}

std::string SecureTransportCertificate::getSubjectName() const {
    return subjectName_;
}

std::vector<std::string> SecureTransportCertificate::getCommonNames() const {
    return commonNames_;
}

std::vector<std::string> SecureTransportCertificate::getSRVNames() const {
    return srvNames_;
}

std::vector<std::string> SecureTransportCertificate::getDNSNames() const {
    return dnsNames_;
}

std::vector<std::string> SecureTransportCertificate::getXMPPAddresses() const {
    return xmppAddresses_;
}

ByteArray SecureTransportCertificate::toDER() const {
    ByteArray der;
    if (certificateHandle_) {
        CFDataRef derData = SecCertificateCopyData(certificateHandle_.get());
        if (derData) {
            try {
                size_t dataSize = boost::numeric_cast<size_t>(CFDataGetLength(derData));
                der.resize(dataSize);
                CFDataGetBytes(derData, CFRangeMake(0,CFDataGetLength(derData)), der.data());
            } catch (...) {
            }
            CFRelease(derData);
        }
    }
    return der;
}

}
