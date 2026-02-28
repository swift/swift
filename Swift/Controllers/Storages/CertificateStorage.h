/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include <Swiften/TLS/Certificate.h>

namespace Swift {
    class CertificateStorage {
        public:
            virtual ~CertificateStorage();

            virtual bool hasCertificate(Certificate::ref certificate) const = 0;
            virtual void addCertificate(Certificate::ref certificate) = 0;
    };

}
