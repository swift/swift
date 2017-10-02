/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
    class SWIFTEN_API CompressRequestSerializer : public ElementSerializer {
        public:
            CompressRequestSerializer();

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> element)  const;
            virtual bool canSerialize(std::shared_ptr<ToplevelElement> element) const;
    };
}
