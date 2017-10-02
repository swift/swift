/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamResumed.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
    class SWIFTEN_API StreamResumedSerializer : public GenericElementSerializer<StreamResumed> {
        public:
            StreamResumedSerializer();

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement>) const;
    };
}
