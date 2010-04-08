/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MUCOccupant_H
#define SWIFTEN_MUCOccupant_H

#include "Swiften/Base/String.h"

namespace Swift {
	class Client;

	class MUCOccupant {
		public:
			MUCOccupant(const String &nick);
			~MUCOccupant();

			String getNick() const;

		private:
			String nick_;
	};
}

#endif
