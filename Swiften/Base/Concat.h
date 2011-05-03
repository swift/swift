/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	template<typename C>
	C concat(const C& c1, const C& c2) {
		C result;
		result.resize(c1.size() + c2.size());
		std::copy(c2.begin(), c2.end(), std::copy(c1.begin(), c1.end(), result.begin()));
		return result;
	}

	template<typename C>
	C concat(const C& c1, const C& c2, const C& c3) {
		C result;
		result.resize(c1.size() + c2.size() + c3.size());
		std::copy(c3.begin(), c3.end(), std::copy(c2.begin(), c2.end(), std::copy(c1.begin(), c1.end(), result.begin())));
		return result;
	}

	template<typename C>
	C concat(const C& c1, const C& c2, const C& c3, const C& c4) {
		C result;
		result.resize(c1.size() + c2.size() + c3.size() + c4.size());
		std::copy(c4.begin(), c4.end(), std::copy(c3.begin(), c3.end(), std::copy(c2.begin(), c2.end(), std::copy(c1.begin(), c1.end(), result.begin()))));
		return result;
	}

	template<typename C>
	C concat(const C& c1, const C& c2, const C& c3, const C& c4, const C& c5) {
		C result;
		result.resize(c1.size() + c2.size() + c3.size() + c4.size() + c5.size());
		std::copy(c5.begin(), c5.end(), std::copy(c4.begin(), c4.end(), std::copy(c3.begin(), c3.end(), std::copy(c2.begin(), c2.end(), std::copy(c1.begin(), c1.end(), result.begin())))));
		return result;
	}

	template<typename C>
	C concat(const C& c1, const C& c2, const C& c3, const C& c4, const C& c5, const C& c6) {
		C result;
		result.resize(c1.size() + c2.size() + c3.size() + c4.size() + c5.size() + c6.size());
		std::copy(c6.begin(), c6.end(), std::copy(c5.begin(), c5.end(), std::copy(c4.begin(), c4.end(), std::copy(c3.begin(), c3.end(), std::copy(c2.begin(), c2.end(), std::copy(c1.begin(), c1.end(), result.begin()))))));
		return result;
	}

	template<typename C>
	C concat(const C& c1, const C& c2, const C& c3, const C& c4, const C& c5, const C& c6, const C& c7) {
		C result;
		result.resize(c1.size() + c2.size() + c3.size() + c4.size() + c5.size() + c6.size() + c7.size());
		std::copy(c7.begin(), c7.end(), std::copy(c6.begin(), c6.end(), std::copy(c5.begin(), c5.end(), std::copy(c4.begin(), c4.end(), std::copy(c3.begin(), c3.end(), std::copy(c2.begin(), c2.end(), std::copy(c1.begin(), c1.end(), result.begin())))))));
		return result;
	}

	template<typename C>
	C concat(const C& c1, const C& c2, const C& c3, const C& c4, const C& c5, const C& c6, const C& c7, const C& c8) {
		C result;
		result.resize(c1.size() + c2.size() + c3.size() + c4.size() + c5.size() + c6.size() + c7.size() + c8.size());
		std::copy(c8.begin(), c8.end(), std::copy(c7.begin(), c7.end(), std::copy(c6.begin(), c6.end(), std::copy(c5.begin(), c5.end(), std::copy(c4.begin(), c4.end(), std::copy(c3.begin(), c3.end(), std::copy(c2.begin(), c2.end(), std::copy(c1.begin(), c1.end(), result.begin()))))))));
		return result;
	}

	template<typename C>
	C concat(const C& c1, const C& c2, const C& c3, const C& c4, const C& c5, const C& c6, const C& c7, const C& c8, const C& c9) {
		C result;
		result.resize(c1.size() + c2.size() + c3.size() + c4.size() + c5.size() + c6.size() + c7.size() + c8.size() + c9.size());
		std::copy(c9.begin(), c9.end(), std::copy(c8.begin(), c8.end(), std::copy(c7.begin(), c7.end(), std::copy(c6.begin(), c6.end(), std::copy(c5.begin(), c5.end(), std::copy(c4.begin(), c4.end(), std::copy(c3.begin(), c3.end(), std::copy(c2.begin(), c2.end(), std::copy(c1.begin(), c1.end(), result.begin())))))))));
		return result;
	}
}
