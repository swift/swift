/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_FOREACH_H
#define SWIFTEN_FOREACH_H

#include <boost/foreach.hpp>

#undef foreach
#define foreach BOOST_FOREACH

#endif
