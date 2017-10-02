/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Whiteboard/WhiteboardServer.h>

#include <iostream>

#include <Swiften/Whiteboard/WhiteboardTransformer.h>

namespace Swift {
    void WhiteboardServer::handleLocalOperationReceived(WhiteboardOperation::ref operation) {
        operations_.push_back(operation);
    }

    WhiteboardOperation::ref WhiteboardServer::handleClientOperationReceived(WhiteboardOperation::ref newOperation) {
        std::list<WhiteboardOperation::ref>::reverse_iterator it;
        if (operations_.empty() || newOperation->getParentID() == operations_.back()->getID()) {
            operations_.push_back(newOperation);
            return newOperation;
        }
        for (it = operations_.rbegin(); it != operations_.rend(); ++it) {
            WhiteboardOperation::ref operation = *it;
            while (newOperation->getParentID() == operation->getParentID()) {
                std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> tResult = WhiteboardTransformer::transform(newOperation, operation);

                if (it == operations_.rbegin()) {
                    operations_.push_back(tResult.second);
                    return tResult.second;
                } else {
                    newOperation = tResult.second;
                    --it;
                    operation = *it;
                }

            }
        }
        return WhiteboardOperation::ref();
    }

    void WhiteboardServer::print() {
        std::list<WhiteboardOperation::ref>::iterator it;
        std::cout << "Server:" << std::endl;
        for(it = operations_.begin(); it != operations_.end(); ++it) {
            std::cout << (*it)->getID() << " " << (*it)->getPos() << std::endl;
        }
    }

}
