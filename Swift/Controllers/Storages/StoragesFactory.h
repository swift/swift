/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
    class Storages;
    class JID;

    class StoragesFactory {
        public:
            virtual ~StoragesFactory() {}

            virtual Storages* createStorages(const JID& profile) const = 0;
    };
}
