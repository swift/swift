/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/VCard.h>

namespace Swift {
    class VCardCollection {
        public:
            virtual ~VCardCollection();

            virtual boost::shared_ptr<VCard> getOwnVCard() const = 0;
            virtual void setOwnVCard(boost::shared_ptr<VCard> vcard) = 0;
    };
}
