/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
    /*
     * @brief This class represents a XEP-0300 <hash/> element.
     */
    class SWIFTEN_API HashElement {
        public:
            HashElement(const std::string& algorithm, const ByteArray& hash) : algorithm_(algorithm), hash_(hash) {
            }

            void setHashValue(const std::string& algorithm, const ByteArray& hash) {
                algorithm_ = algorithm;
                hash_ = hash;
            }

            const std::string& getAlgorithm() const {
                return algorithm_;
            }

            const ByteArray& getHashValue() const {
                return hash_;
            }

            bool operator==(const HashElement& rhs) const {
                return (algorithm_ == rhs.algorithm_) && (hash_ == rhs.hash_);
            }


        private:
            std::string algorithm_;
            ByteArray hash_;
    };
}
