/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Jingle/IncomingJingleSession.h>

namespace Swift {

IncomingJingleSession::IncomingJingleSession(const std::string& id, const std::vector<JingleContent::ref>& contents) : JingleSession(id, contents) {

}

}
