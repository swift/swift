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
    template<typename T>
    class SWIFTEN_API GenericElementSerializer : public ElementSerializer {
        public:
            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> element) const = 0;

            virtual bool canSerialize(std::shared_ptr<ToplevelElement> element) const {
                return !!std::dynamic_pointer_cast<T>(element);
            }
    };
}
