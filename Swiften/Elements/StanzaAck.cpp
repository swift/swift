/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
