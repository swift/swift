/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/Controllers/Contact.h>

namespace Swift {

Contact::Contact() {
}

Contact::Contact(const std::string& name, const JID& jid, StatusShow::Type statusType, const boost::filesystem::path& path) : name(name), jid(jid), statusType(statusType), avatarPath(path) {
}

}
