/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// Work around for the boost::signals / Qt signals keyword clash.
// Based on an example from Frank Hess, improved by Niels Dekker

#pragma once

#if defined(signals) && defined(Q_SIGNALS) && !defined(QT_MOC_CPP)
#undef signals
#define signals signals
#endif

#include <boost/signal.hpp>

namespace boost {
	namespace bsignals = signals;
}

#if defined(signals) && defined(Q_SIGNALS) && !defined(QT_MOC_CPP)
#undef signals
#define signals Q_SIGNALS
#endif
