/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardOperation.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
    class IQRouter;
    class WhiteboardPayload;

    class SWIFTEN_API WhiteboardSession {
    public:
        typedef std::shared_ptr<WhiteboardSession> ref;

    public:
        WhiteboardSession(const JID& jid, IQRouter* router);
        virtual ~WhiteboardSession();
        void handleIncomingAction(std::shared_ptr<WhiteboardPayload> payload);
        void sendElement(const WhiteboardElement::ref element);
        virtual void sendOperation(WhiteboardOperation::ref operation) = 0;
        void cancel();
        const JID& getTo() const;

    public:
        boost::signals2::signal< void(const WhiteboardElement::ref element)> onElementReceived;
        boost::signals2::signal< void(const WhiteboardOperation::ref operation)> onOperationReceived;
        boost::signals2::signal< void(const JID& contact)> onSessionTerminated;
        boost::signals2::signal< void(const JID& contact)> onRequestAccepted;
        boost::signals2::signal< void(const JID& contact)> onRequestRejected;

    private:
        virtual void handleIncomingOperation(WhiteboardOperation::ref operation) = 0;

    protected:
        void sendPayload(std::shared_ptr<WhiteboardPayload> payload);

        JID toJID_;
        IQRouter* router_;
        std::string lastOpID;
        IDGenerator idGenerator;
    };
}
