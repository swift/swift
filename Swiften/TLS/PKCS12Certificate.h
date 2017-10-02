/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/CertificateWithKey.h>

namespace Swift {
    class SWIFTEN_API PKCS12Certificate : public Swift::CertificateWithKey {
        public:
            PKCS12Certificate() {}

            PKCS12Certificate(const boost::filesystem::path& filename, const SafeByteArray& password) : password_(password) {
                readByteArrayFromFile(data_, filename);
            }

            virtual ~PKCS12Certificate() {}

            virtual bool isNull() const {
                return data_.empty();
            }

            virtual bool isPrivateKeyExportable() const {
/////Hopefully a PKCS12 is never missing a private key
                return true;
            }

            virtual const std::string& getCertStoreName() const {
/////                assert(0);
                throw std::exception();
            }

            virtual const std::string& getCertName() const {
                /* We can return the original filename instead, if we care */
/////                assert(0);
                throw std::exception();
            }

            virtual const ByteArray& getData() const {
                return data_;
            }

            void setData(const ByteArray& data) {
                data_ = data;
            }

            virtual const SafeByteArray& getPassword() const {
                return password_;
            }

        private:
            ByteArray data_;
            SafeByteArray password_;
    };
}
