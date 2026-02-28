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
    onBeginAdd(static_cast<int>(list_.size()));
    list_.push_back(jid);
    states_[jid] = state;
    onAdded();
}

void Chattables::setState(const JID& jid, State state) {
    auto stateIter = states_.find(jid);
    if (stateIter == states_.end()) {
        return;
    }
    stateIter->second = state;
    auto listPos = static_cast<int>(std::distance(list_.begin(), std::find(list_.begin(), list_.end(), jid)));
    onChanged(jid, listPos);
}

}
