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
