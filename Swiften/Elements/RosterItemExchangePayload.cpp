/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Elements/RosterItemExchangePayload.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

RosterItemExchangePayload::Item::Item(Action action) : action(action) {
}

RosterItemExchangePayload::RosterItemExchangePayload() {
}

}
