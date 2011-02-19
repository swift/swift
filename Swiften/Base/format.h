/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/format.hpp>

namespace Swift {
	inline boost::format format(const std::string& s) {
		using namespace boost::io;
		boost::format fmter(s);
		fmter.exceptions( all_error_bits ^ ( too_many_args_bit | too_few_args_bit )  );
		return fmter;
	}
}
