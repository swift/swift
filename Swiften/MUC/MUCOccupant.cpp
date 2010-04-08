/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/MUC/MUCOccupant.h"

namespace Swift {

MUCOccupant::MUCOccupant(const String &nick) : nick_(nick) {
}

MUCOccupant::~MUCOccupant() {
}

String MUCOccupant::getNick() const {
	return nick_;
}

}
