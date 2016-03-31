/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
    class SWIFTEN_API StreamManagementEnabledSerializer : public GenericElementSerializer<StreamManagementEnabled> {
        public:
            StreamManagementEnabledSerializer();

            virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const;
    };
}
