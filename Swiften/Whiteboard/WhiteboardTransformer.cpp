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

#include <Swiften/Whiteboard/WhiteboardTransformer.h>

#include <memory>

namespace Swift {
    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardOperation::ref clientOp, WhiteboardOperation::ref serverOp) {
        WhiteboardInsertOperation::ref clientInsert = std::dynamic_pointer_cast<WhiteboardInsertOperation>(clientOp);
        WhiteboardInsertOperation::ref serverInsert = std::dynamic_pointer_cast<WhiteboardInsertOperation>(serverOp);
        WhiteboardUpdateOperation::ref clientUpdate = std::dynamic_pointer_cast<WhiteboardUpdateOperation>(clientOp);
        WhiteboardUpdateOperation::ref serverUpdate = std::dynamic_pointer_cast<WhiteboardUpdateOperation>(serverOp);
        WhiteboardDeleteOperation::ref clientDelete = std::dynamic_pointer_cast<WhiteboardDeleteOperation>(clientOp);
        WhiteboardDeleteOperation::ref serverDelete = std::dynamic_pointer_cast<WhiteboardDeleteOperation>(serverOp);
        if (clientInsert && serverInsert) {
            return transform(clientInsert, serverInsert);
        } else if (clientUpdate && serverUpdate) {
            return transform(clientUpdate, serverUpdate);
        } else if (clientInsert && serverUpdate) {
            return transform(clientInsert, serverUpdate);
        } else if (clientUpdate && serverInsert) {
            return transform(clientUpdate, serverInsert);
        } else if (clientDelete && serverDelete) {
            return transform(clientDelete, serverDelete);
        } else if (clientInsert && serverDelete) {
            return transform(clientInsert, serverDelete);
        } else if (clientDelete && serverInsert) {
            return transform(clientDelete, serverInsert);
        } else if (clientUpdate && serverDelete) {
            return transform(clientUpdate, serverDelete);
        } else if (clientDelete && serverUpdate) {
            return transform(clientDelete, serverUpdate);
        } else {
            return std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref>();
        }
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardInsertOperation::ref clientOp, WhiteboardInsertOperation::ref serverOp) {
        std::pair<WhiteboardInsertOperation::ref, WhiteboardInsertOperation::ref> result;
        result.first = std::make_shared<WhiteboardInsertOperation>(*serverOp);
        result.first->setParentID(clientOp->getID());
        result.second = std::make_shared<WhiteboardInsertOperation>(*clientOp);
        result.second->setParentID(serverOp->getID());
        if (clientOp->getPos() <= serverOp->getPos()) {
            result.first->setPos(result.first->getPos()+1);
        } else {
            result.second->setPos(result.second->getPos()+1);
        }
        return result;
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardUpdateOperation::ref clientOp, WhiteboardUpdateOperation::ref serverOp) {
        std::pair<WhiteboardUpdateOperation::ref, WhiteboardUpdateOperation::ref> result;
        result.first = std::make_shared<WhiteboardUpdateOperation>(*serverOp);
        result.first->setParentID(clientOp->getID());

        if (clientOp->getPos() == serverOp->getPos()) {
            result.second = std::make_shared<WhiteboardUpdateOperation>(*serverOp);
            result.second->setID(clientOp->getID());
            result.second->setParentID(serverOp->getID());
        } else {
            result.second = std::make_shared<WhiteboardUpdateOperation>(*clientOp);
            result.second->setParentID(serverOp->getID());
        }

        if (clientOp->getPos() < serverOp->getPos() && clientOp->getNewPos() > serverOp->getPos()) {
            result.first->setPos(result.first->getPos()-1);
            if (clientOp->getNewPos() >= serverOp->getNewPos()) {
                result.first->setNewPos(result.first->getNewPos()-1);
            }
        } else if (clientOp->getNewPos() >= serverOp->getNewPos()) {
            result.first->setNewPos(result.first->getNewPos()-1);
        }

        if (serverOp->getPos() < clientOp->getPos() && serverOp->getNewPos() > clientOp->getPos()) {
            result.second->setPos(result.second->getPos()-1);
            if (serverOp->getNewPos() >= clientOp->getNewPos()) {
                result.second->setNewPos(result.second->getNewPos()-1);
            }
        } else if (serverOp->getNewPos() >= clientOp->getNewPos()) {
            result.second->setNewPos(result.second->getNewPos()-1);
        }
        return result;
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardUpdateOperation::ref clientOp, WhiteboardInsertOperation::ref serverOp) {
        std::pair<WhiteboardInsertOperation::ref, WhiteboardUpdateOperation::ref> result;
        result.first = std::make_shared<WhiteboardInsertOperation>(*serverOp);
        result.first->setParentID(clientOp->getID());
        result.second = std::make_shared<WhiteboardUpdateOperation>(*clientOp);
        result.second->setParentID(serverOp->getID());
        if (serverOp->getPos() <= clientOp->getPos()) {
            result.second->setPos(result.second->getPos()+1);
        }
        return result;
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardInsertOperation::ref clientOp, WhiteboardUpdateOperation::ref serverOp) {
        std::pair<WhiteboardUpdateOperation::ref, WhiteboardInsertOperation::ref> result;
        result.first = std::make_shared<WhiteboardUpdateOperation>(*serverOp);
        result.first->setParentID(clientOp->getID());
        result.second = std::make_shared<WhiteboardInsertOperation>(*clientOp);
        result.second->setParentID(serverOp->getID());
        if (serverOp->getPos() >= clientOp->getPos()) {
            result.first->setPos(result.first->getPos()+1);
        }
        return result;
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardDeleteOperation::ref clientOp, WhiteboardDeleteOperation::ref serverOp) {
        std::pair<WhiteboardDeleteOperation::ref, WhiteboardDeleteOperation::ref> result;
        result.first = std::make_shared<WhiteboardDeleteOperation>(*serverOp);
        result.first->setParentID(clientOp->getID());
        result.second = std::make_shared<WhiteboardDeleteOperation>(*clientOp);
        result.second->setParentID(serverOp->getID());
        if (clientOp->getPos() == -1) {
            result.second->setPos(-1);
        }
        if (serverOp->getPos() == -1) {
            result.first->setPos(-1);
        }
        if (clientOp->getPos() < serverOp->getPos()) {
            result.first->setPos(result.first->getPos()-1);
        } else if (clientOp->getPos() > serverOp->getPos()) {
            result.second->setPos(result.second->getPos()-1);
        } else {
            result.first->setPos(-1);
            result.second->setPos(-1);
        }
        return result;
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardInsertOperation::ref clientOp, WhiteboardDeleteOperation::ref serverOp) {
        std::pair<WhiteboardDeleteOperation::ref, WhiteboardInsertOperation::ref> result;
        result.first = std::make_shared<WhiteboardDeleteOperation>(*serverOp);
        result.first->setParentID(clientOp->getID());
        result.second = std::make_shared<WhiteboardInsertOperation>(*clientOp);
        result.second->setParentID(serverOp->getID());
        if (clientOp->getPos() <= serverOp->getPos()) {
            result.first->setPos(result.first->getPos()+1);
        } else if (serverOp->getPos() != -1) {
            result.second->setPos(result.second->getPos()-1);
        }
        return result;
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardDeleteOperation::ref clientOp, WhiteboardInsertOperation::ref serverOp) {
        std::pair<WhiteboardInsertOperation::ref, WhiteboardDeleteOperation::ref> result;
        result.first = std::make_shared<WhiteboardInsertOperation>(*serverOp);
        result.first->setParentID(clientOp->getID());
        result.second = std::make_shared<WhiteboardDeleteOperation>(*clientOp);
        result.second->setParentID(serverOp->getID());
        if (serverOp->getPos() <= clientOp->getPos()) {
            result.second->setPos(result.second->getPos()+1);
        } else if (clientOp->getPos() != -1) {
            result.first->setPos(result.first->getPos()-1);
        }
        return result;
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardUpdateOperation::ref clientOp, WhiteboardDeleteOperation::ref serverOp) {
        std::pair<WhiteboardDeleteOperation::ref, WhiteboardOperation::ref> result;
        result.first = std::make_shared<WhiteboardDeleteOperation>(*serverOp);
        result.first->setParentID(clientOp->getID());
        WhiteboardUpdateOperation::ref updateOp = std::make_shared<WhiteboardUpdateOperation>(*clientOp);
        result.second = updateOp;
        result.second->setParentID(serverOp->getID());
        if (clientOp->getPos() == serverOp->getPos()) {
            WhiteboardDeleteOperation::ref deleteOp = std::make_shared<WhiteboardDeleteOperation>();
            result.second = deleteOp;
            result.second->setPos(-1);
            result.second->setID(clientOp->getID());
            result.second->setParentID(serverOp->getID());
            deleteOp->setElementID(serverOp->getElementID());
        } else if (clientOp->getPos() > serverOp->getPos() && clientOp->getNewPos() <= serverOp->getPos()) {
            result.second->setPos(result.second->getPos()-1);
        } else if (clientOp->getPos() < serverOp->getPos() && clientOp->getNewPos() >= serverOp->getPos()) {
            updateOp->setNewPos(updateOp->getNewPos()-1);
        } else if (clientOp->getPos() > serverOp->getPos()) {
            result.second->setPos(result.second->getPos()-1);
            updateOp->setNewPos(updateOp->getNewPos()-1);
        }
        return result;
    }

    std::pair<WhiteboardOperation::ref, WhiteboardOperation::ref> WhiteboardTransformer::transform(WhiteboardDeleteOperation::ref clientOp, WhiteboardUpdateOperation::ref serverOp) {
        std::pair<WhiteboardOperation::ref, WhiteboardDeleteOperation::ref> result;
        WhiteboardUpdateOperation::ref updateOp = std::make_shared<WhiteboardUpdateOperation>(*serverOp);
        result.first = updateOp;
        result.first->setParentID(clientOp->getID());
        result.second = std::make_shared<WhiteboardDeleteOperation>(*clientOp);
        result.second->setParentID(serverOp->getID());
        if (clientOp->getPos() == serverOp->getPos()) {
            WhiteboardDeleteOperation::ref deleteOp = std::make_shared<WhiteboardDeleteOperation>();
            result.first = deleteOp;
            result.first->setPos(-1);
            result.first->setID(serverOp->getID());
            result.first->setParentID(clientOp->getID());
            deleteOp->setElementID(clientOp->getElementID());
        } else if (clientOp->getPos() < serverOp->getPos() && clientOp->getPos() >= serverOp->getNewPos()) {
            result.first->setPos(result.first->getPos()-1);
        } else if (clientOp->getPos() > serverOp->getPos() && clientOp->getPos() <= serverOp->getNewPos()) {
            updateOp->setNewPos(updateOp->getNewPos()-1);
        } else if (clientOp->getPos() < serverOp->getPos()) {
            result.first->setPos(result.first->getPos()-1);
            updateOp->setNewPos(updateOp->getNewPos()-1);
        }
        return result;
    }
}
