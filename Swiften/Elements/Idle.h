/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {

    class SWIFTEN_API Idle : public Payload {
    public:
        typedef std::shared_ptr<Idle> ref;

    public:
        Idle() {}
        Idle(boost::posix_time::ptime since) : since_(since) {
        }

        void setSince(boost::posix_time::ptime since) {
            since_ = since;
        }

        boost::posix_time::ptime getSince() const {
            return since_;
        }

    private:
        boost::posix_time::ptime since_;
    };

}
