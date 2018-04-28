/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/Chattables.h>

namespace Swift {

const std::vector<JID>& Chattables::get() const {
    return list_;
}

const Chattables::State& Chattables::getState(const JID& jid) const {
    auto it = states_.find(jid);
    return it != states_.end() ? it->second : unknown_;
}

void Chattables::addJID(const JID& jid, State::Type type) {
    if (states_.find(jid) != states_.end()) {
        return;
    }
    State state;
    state.type = type;
    state.jid = jid;
    list_.push_back(jid);
    states_[jid] = state;
    onAdded(jid);
}

void Chattables::setState(const JID& jid, State state) {
    states_[jid] = state;
    onChanged(jid);
}

}
