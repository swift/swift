/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
    class SWIFTEN_API StreamManagementEnabledSerializer : public GenericElementSerializer<StreamManagementEnabled> {
        public:
            StreamManagementEnabledSerializer();

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement>) const;
    };
}
