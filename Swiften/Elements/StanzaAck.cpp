/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Elements/StanzaAck.h>

#include <boost/numeric/conversion/cast.hpp>

using namespace Swift;

StanzaAck::~StanzaAck() {
}

void StanzaAck::setHandledStanzasCount(int i) {
	handledStanzasCount = boost::numeric_cast<unsigned int>(i);
	valid = true;
}
