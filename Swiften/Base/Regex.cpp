/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Base/Regex.h>

#include <string>

#include <boost/regex.hpp>

namespace Swift {

    namespace Regex {
            std::string escape(const std::string& source) {
                // escape regex special characters: ^.$| etc
                // these need to be escaped: [\^\$\|.........]
                // and then C++ requires '\' to be escaped, too....
                static const boost::regex esc("([\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\{\\}\\\\])");
                // matched character should be prepended with '\'
                // replace matched special character with \\\1
                // and escape once more for C++ rules...
                static const std::string rep("\\\\\\1");
                return boost::regex_replace(source, esc, rep);
            }

    }

}

