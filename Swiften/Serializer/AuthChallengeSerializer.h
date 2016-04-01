/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
    class SWIFTEN_API AuthChallengeSerializer : public GenericElementSerializer<AuthChallenge> {
        public:
            AuthChallengeSerializer();

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> element)  const;
    };
}
