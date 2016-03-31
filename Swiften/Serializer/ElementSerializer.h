/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class ElementSerializer {
        public:
            virtual ~ElementSerializer();

            virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement> element) const = 0;
            virtual bool canSerialize(boost::shared_ptr<ToplevelElement> element) const = 0;
    };
}
