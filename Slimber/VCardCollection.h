/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Elements/VCard.h>

namespace Swift {
    class VCardCollection {
        public:
            virtual ~VCardCollection();

            virtual std::shared_ptr<VCard> getOwnVCard() const = 0;
            virtual void setOwnVCard(std::shared_ptr<VCard> vcard) = 0;
    };
}
