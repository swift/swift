/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/signals2.hpp>

namespace Swift {
    class StanzaEvent {
        public:
            StanzaEvent() : time_(boost::posix_time::microsec_clock::universal_time()) {concluded_ = false;}
            virtual ~StanzaEvent() {}
            void conclude() {concluded_ = true; onConclusion();}
            /** Do not call this directly from outside the class.
             * If you connect to this signal, you *must* disconnect from it manually. */
            boost::signals2::signal<void()> onConclusion;
            bool getConcluded() {return concluded_;}
            boost::posix_time::ptime getTime() {return time_;}
        private:
            bool concluded_;
            boost::posix_time::ptime time_;
    };
}
