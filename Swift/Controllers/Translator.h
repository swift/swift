/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	class Translator {
		public:
			virtual ~Translator();

			virtual std::string translate(const std::string& text, const std::string& context) = 0;

			static void setInstance(Translator* translator);

			static Translator* getInstance() {
				return translator;
			}

		private:
			static Translator* translator;
	};
}
