/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cassert>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API StatusShow : public Payload {
        public:
            enum Type { Online, Away, FFC, XA, DND, None };

            StatusShow(const Type& type = Online);

            void setType(const Type& type) {
                type_ = type;
            }

            const Type& getType() const {
                return type_;
            }

            /**
             * Can be used for rough ordering of Types.
             * Greater magnitude = more available.
             */
            static int typeToAvailabilityOrdering(Type type) {
                switch (type) {
                    case Online: return 4;
                    case FFC: return 5;
                    case Away: return 2;
                    case XA: return 1;
                    case DND: return 3;
                    case None: return 0;
                }
                assert(false);
                return 0;
            }

        private:
            Type type_;
    };
}
