/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/AuthResponse.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
    class SWIFTEN_API AuthResponseSerializer : public GenericElementSerializer<AuthResponse> {
        public:
            AuthResponseSerializer();

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> element)  const;
    };
}
