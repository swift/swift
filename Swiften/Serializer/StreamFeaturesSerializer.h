/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamFeatures.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
    class SWIFTEN_API StreamFeaturesSerializer : public GenericElementSerializer<StreamFeatures> {
        public:
            StreamFeaturesSerializer();

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> element)  const;
    };
}
