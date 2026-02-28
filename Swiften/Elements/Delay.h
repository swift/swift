/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API Delay : public Payload {
        public:
            Delay() {}
            Delay(const boost::posix_time::ptime& time, const JID& from = JID()) : time_(time), from_(from) {}

            const boost::posix_time::ptime& getStamp() const {return time_;}
            void setStamp(const boost::posix_time::ptime& time) {time_ = time;}

            const boost::optional<JID>& getFrom() const {return from_;}
            void setFrom(const JID& from) {from_ = from;}

        private:
            boost::posix_time::ptime time_;
            boost::optional<JID> from_;
    };
}
