/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/RosterItemExchangePayload.h>

namespace Swift {

RosterItemExchangePayload::Item::Item(Action action) : action(action) {
}

RosterItemExchangePayload::RosterItemExchangePayload() {
}

}
