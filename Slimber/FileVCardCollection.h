/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/filesystem.hpp>

#include <Slimber/VCardCollection.h>

namespace Swift {
    class FileVCardCollection : public VCardCollection {
        public:
            FileVCardCollection(boost::filesystem::path dir);

            std::shared_ptr<VCard> getOwnVCard() const;
            void setOwnVCard(std::shared_ptr<VCard> vcard);

        private:
            boost::filesystem::path vcardsPath;
    };
}
