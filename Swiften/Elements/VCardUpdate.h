/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API VCardUpdate : public Payload {
        public:
            VCardUpdate(const std::string& photoHash = "") : photoHash_(photoHash) {}

            void setPhotoHash(const std::string& photoHash) { photoHash_ = photoHash; }
            const std::string& getPhotoHash() const { return photoHash_; }

        private:
            std::string photoHash_;
    };
}
