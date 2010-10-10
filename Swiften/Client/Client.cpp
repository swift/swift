/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/Client.h"

namespace Swift {

Client::Client(const JID& jid, const String& password) : CoreClient(jid, password) {
}

}
