/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamError.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
    class SWIFTEN_API StreamErrorSerializer : public GenericElementSerializer<StreamError> {
        public:
            StreamErrorSerializer();

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> error) const;
    };
}
