/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace Swift {
    class Translator {
        public:
            virtual ~Translator();

            virtual std::string translate(const std::string& text, const std::string& context) = 0;

            virtual std::string ptimeToHumanReadableString(const boost::posix_time::ptime& time) = 0;

            static void setInstance(Translator* translator);

            static Translator* getInstance() {
                return translator;
            }

        private:
            static Translator* translator;
    };
}
