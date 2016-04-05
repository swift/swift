/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/JID/JID.h>

namespace Swift {

    class ClientBlockListManager;

    class BlockListEditorWidget {
        public:
            virtual ~BlockListEditorWidget() {}

            virtual void show() = 0;
            virtual void hide() = 0;

            virtual void setCurrentBlockList(const std::vector<JID>& blockedJIDs) = 0;
            virtual void setBusy(bool isBusy) = 0;
            virtual void setError(const std::string&) = 0;

            virtual std::vector<JID> getCurrentBlockList() const = 0;

            boost::signals2::signal<void (const std::vector<JID>& /* blockedJID */)> onSetNewBlockList;
    };

}
