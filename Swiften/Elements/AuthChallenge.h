/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API AuthChallenge : public ToplevelElement {
        public:
            AuthChallenge() {
            }

            AuthChallenge(const std::vector<unsigned char>& value) : value(value) {
            }

            const boost::optional< std::vector<unsigned char> >& getValue() const {
                return value;
            }

            void setValue(const std::vector<unsigned char>& value) {
                this->value = boost::optional<std::vector<unsigned char> >(value);
            }

        private:
            boost::optional< std::vector<unsigned char> > value;
    };
}
