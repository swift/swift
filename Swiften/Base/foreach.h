/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/foreach.hpp>

#undef foreach
#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH
