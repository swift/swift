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

#include <Swiften/Whiteboard/WhiteboardClient.h>

#include <iostream>
#include <memory>

#include <Swiften/Whiteboard/WhiteboardTransformer.h>

namespace Swift {
    WhiteboardOperation::ref WhiteboardClient::handleLocalOperationReceived(WhiteboardOperation::ref operation) {
        localOperations_.push_back(operation);

        WhiteboardOperation::ref op;
        WhiteboardInsertOperation::ref insertOp = std::dynamic_pointer_cast<WhiteboardInsertOperation>(operation);
        if (insertOp) {
            op = std::make_shared<WhiteboardInsertOperation>(*insertOp);
        }
        WhiteboardUpdateOperation::ref updateOp = std::dynamic_pointer_cast<WhiteboardUpdateOperation>(operation);
        if (updateOp) {
            op = std::make_shared<WhiteboardUpdateOperation>(*updateOp);
        }
        WhiteboardDeleteOperation::ref deleteOp = std::dynamic_pointer_cast<WhiteboardDeleteOperation>(operation);
        if (deleteOp) {
            op = std::make_shared<WhiteboardDeleteOperation>(*deleteOp);
        }

        if (!bridge_.empty()) {
            op->setParentID(bridge_.back()->getID());
        }
        bridge_.push_back(op);

        if (lastSentOperationID_.empty())
        {
            WhiteboardInsertOperation::ref insertOp = std::dynamic_pointer_cast<WhiteboardInsertOperation>(operation);
            if (insertOp) {
                op = std::make_shared<WhiteboardInsertOperation>(*insertOp);
            }
            WhiteboardUpdateOperation::ref updateOp = std::dynamic_pointer_cast<WhiteboardUpdateOperation>(operation);
            if (updateOp) {
                op = std::make_shared<WhiteboardUpdateOperation>(*updateOp);
            }
            WhiteboardDeleteOperation::ref deleteOp = std::dynamic_pointer_cast<WhiteboardDeleteOperation>(operation);
            if (deleteOp) {
                op = std::make_shared<WhiteboardDeleteOperation>(*deleteOp);
            }


            if (!serverOperations_.empty()) {
                op->setParentID(serverOperations_.back()->getID());
            }
            lastSentOperationID_ = operation->getID();
            return op;
        } else {
            return WhiteboardOperation::ref();
        }
    }

    WhiteboardClient::Result WhiteboardClient::handleServerOperationReceived(WhiteboardOperation::ref operation) {
        serverOperations_.push_back(operation);
        Result result;
//        if (localOperations_.empty()) {// || localOperations_.back()->getID() == operation->getParentID()) {
        //Situation where client and server are in sync
        if (localOperations_.size() == serverOperations_.size()-1) {
            localOperations_.push_back(operation);
//            clientOp = operation;
            result.client = operation;
        } else if (lastSentOperationID_ == operation->getID()) {
            //Client received confirmation about own operation and it sends next operation to server
            if (!bridge_.empty() && lastSentOperationID_ == bridge_.front()->getID()) {
                bridge_.erase(bridge_.begin());
            }

            if (!bridge_.empty() && (bridge_.front())->getParentID() == lastSentOperationID_) {
                lastSentOperationID_ = (bridge_.front())->getID();
                result.server = bridge_.front();
            }
            if (!result.server) {
                lastSentOperationID_.clear();
            }
        } else {
            std::list<WhiteboardOperation::ref>::iterator it = bridge_.begin();
            std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> opPair;
            WhiteboardOperation::ref temp;
            opPair = WhiteboardTransformer::transform(*it, operation);
            temp = opPair.first;

            *it = opPair.second;
            std::string previousID = (*it)->getID();
            ++it;
            for (; it != bridge_.end(); ++it) {
                opPair = WhiteboardTransformer::transform(*it, temp);
                temp = opPair.first;
                *it = opPair.second;
                (*it)->setParentID(previousID);
                previousID = (*it)->getID();
            }

            temp->setParentID(localOperations_.back()->getID());
            localOperations_.push_back(temp);
            result.client = temp;
        }

        return result;
    }

    void WhiteboardClient::print() {
        std::list<WhiteboardOperation::ref>::iterator it;
        std::cout << "Client" << std::endl;
        for(it = localOperations_.begin(); it != localOperations_.end(); ++it) {
            std::cout << (*it)->getID() << " " << (*it)->getPos() << std::endl;
        }

        std::cout << "Server" << std::endl;
        for(it = serverOperations_.begin(); it != serverOperations_.end(); ++it) {
            std::cout << (*it)->getID() << " " << (*it)->getPos() << std::endl;
        }
    }
}
