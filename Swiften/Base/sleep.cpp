/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/sleep.h>

#include <boost/thread.hpp>
#include <boost/version.hpp>

namespace Swift {

void sleep(unsigned int msecs) {
	boost::xtime xt;
#if BOOST_VERSION >= 105000
	boost::xtime_get(&xt, boost::TIME_UTC_);
#else
	boost::xtime_get(&xt, boost::TIME_UTC);
#endif
	xt.nsec += msecs*1000000;
	boost::thread::sleep(xt);
}

}
