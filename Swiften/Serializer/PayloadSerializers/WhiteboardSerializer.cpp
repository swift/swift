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

#include <Swiften/Serializer/PayloadSerializers/WhiteboardSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/Whiteboard/WhiteboardDeleteOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardUpdateOperation.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {
    void WhiteboardElementSerializingVisitor::visit(WhiteboardLineElement& line) {
        element = std::make_shared<XMLElement>("line");
        try {
            element->setAttribute("x1", std::to_string(line.x1()));
            element->setAttribute("y1", std::to_string(line.y1()));
            element->setAttribute("x2", std::to_string(line.x2()));
            element->setAttribute("y2", std::to_string(line.y2()));
            element->setAttribute("id", line.getID());
            element->setAttribute("stroke", line.getColor().toHex());
            element->setAttribute("stroke-width", std::to_string(line.getPenWidth()));
            element->setAttribute("opacity", alphaToOpacity(line.getColor().getAlpha()));
        } catch (boost::bad_lexical_cast&) {
        }
    }

    void WhiteboardElementSerializingVisitor::visit(WhiteboardFreehandPathElement& path) {
        element = std::make_shared<XMLElement>("path");
        element->setAttribute("id", path.getID());
        element->setAttribute("stroke", path.getColor().toHex());
        try {
            element->setAttribute("stroke-width", std::to_string(path.getPenWidth()));
            element->setAttribute("opacity", alphaToOpacity(path.getColor().getAlpha()));
            std::string pathData;
            if (path.getPoints().size() != 0) {
                std::vector<std::pair<int, int> >::const_iterator it = path.getPoints().begin();
                pathData = "M"+std::to_string(it->first)+" "+std::to_string(it->second)+"L";
                for (; it != path.getPoints().end(); ++it) {
                    pathData += std::to_string(it->first)+" "+std::to_string(it->second)+" ";
                }
            }
            element->setAttribute("d", pathData);
        } catch (boost::bad_lexical_cast&) {
        }
    }

    void WhiteboardElementSerializingVisitor::visit(WhiteboardRectElement& rect) {
        element = std::make_shared<XMLElement>("rect");
        try {
            element->setAttribute("x", std::to_string(rect.getX()));
            element->setAttribute("y", std::to_string(rect.getY()));
            element->setAttribute("width", std::to_string(rect.getWidth()));
            element->setAttribute("height", std::to_string(rect.getHeight()));
            element->setAttribute("id", rect.getID());
            element->setAttribute("stroke", rect.getPenColor().toHex());
            element->setAttribute("fill", rect.getBrushColor().toHex());;
            element->setAttribute("stroke-width", std::to_string(rect.getPenWidth()));
            element->setAttribute("opacity", alphaToOpacity(rect.getPenColor().getAlpha()));
            element->setAttribute("fill-opacity", alphaToOpacity(rect.getBrushColor().getAlpha()));
        } catch (boost::bad_lexical_cast&) {
        }
    }

    void WhiteboardElementSerializingVisitor::visit(WhiteboardPolygonElement& polygon) {
        element = std::make_shared<XMLElement>("polygon");
        try {
            element->setAttribute("id", polygon.getID());
            element->setAttribute("stroke", polygon.getPenColor().toHex());
            element->setAttribute("fill", polygon.getBrushColor().toHex());;
            element->setAttribute("stroke-width", std::to_string(polygon.getPenWidth()));
            element->setAttribute("opacity", alphaToOpacity(polygon.getPenColor().getAlpha()));
            element->setAttribute("fill-opacity", alphaToOpacity(polygon.getBrushColor().getAlpha()));
            std::string points;
            std::vector<std::pair<int, int> >::const_iterator it = polygon.getPoints().begin();
            for (; it != polygon.getPoints().end(); ++it) {
                points += std::to_string(it->first)+","+std::to_string(it->second)+" ";
            }
            element->setAttribute("points", points);
        } catch (boost::bad_lexical_cast&) {
        }
    }

    void WhiteboardElementSerializingVisitor::visit(WhiteboardTextElement& text) {
        element = std::make_shared<XMLElement>("text");
        try {
            element->setAttribute("x", std::to_string(text.getX()));
            element->setAttribute("y", std::to_string(text.getY()));
            element->setAttribute("font-size", std::to_string(text.getSize()));
            element->setAttribute("id", text.getID());
            element->setAttribute("fill", text.getColor().toHex());
            element->setAttribute("opacity", alphaToOpacity(text.getColor().getAlpha()));
            element->addNode(std::make_shared<XMLTextNode>(text.getText()));
        } catch (boost::bad_lexical_cast&) {
        }
    }

     void WhiteboardElementSerializingVisitor::visit(WhiteboardEllipseElement& ellipse) {
        element = std::make_shared<XMLElement>("ellipse");
        try {
            element->setAttribute("cx", std::to_string(ellipse.getCX()));
            element->setAttribute("cy", std::to_string(ellipse.getCY()));
            element->setAttribute("rx", std::to_string(ellipse.getRX()));
            element->setAttribute("ry", std::to_string(ellipse.getRY()));
            element->setAttribute("id", ellipse.getID());
            element->setAttribute("stroke", ellipse.getPenColor().toHex());
            element->setAttribute("fill", ellipse.getBrushColor().toHex());;
            element->setAttribute("stroke-width", std::to_string(ellipse.getPenWidth()));
            element->setAttribute("opacity", alphaToOpacity(ellipse.getPenColor().getAlpha()));
            element->setAttribute("fill-opacity", alphaToOpacity(ellipse.getBrushColor().getAlpha()));
        } catch (boost::bad_lexical_cast&) {
        }
    }

    XMLElement::ref WhiteboardElementSerializingVisitor::getResult() const {
        return element;
    }

    std::string WhiteboardElementSerializingVisitor::alphaToOpacity(int alpha) const {
        int opacity = 100*alpha/254;
        if (opacity == 100) {
            return "1";
        } else {
            return "."+std::to_string(opacity);
        }
    }

    std::string WhiteboardSerializer::serializePayload(std::shared_ptr<WhiteboardPayload> payload) const {
        XMLElement element("wb", "http://swift.im/whiteboard");
        if (payload->getType() == WhiteboardPayload::Data) {
            XMLElement::ref operationNode = std::make_shared<XMLElement>("operation");
            WhiteboardElementSerializingVisitor visitor;
//            payload->getElement()->accept(visitor);
            WhiteboardInsertOperation::ref insertOp = std::dynamic_pointer_cast<WhiteboardInsertOperation>(payload->getOperation());
            if (insertOp) {
                try {
                    operationNode->setAttribute("type", "insert");
                    operationNode->setAttribute("pos", std::to_string(insertOp->getPos()));
                    operationNode->setAttribute("id", insertOp->getID());
                    operationNode->setAttribute("parentid", insertOp->getParentID());
                } catch (boost::bad_lexical_cast&) {
                }
                insertOp->getElement()->accept(visitor);
                operationNode->addNode(visitor.getResult());
            }
            WhiteboardUpdateOperation::ref updateOp = std::dynamic_pointer_cast<WhiteboardUpdateOperation>(payload->getOperation());
            if (updateOp) {
                try {
                    operationNode->setAttribute("type", "update");
                    operationNode->setAttribute("pos", std::to_string(updateOp->getPos()));
                    operationNode->setAttribute("id", updateOp->getID());
                    operationNode->setAttribute("parentid", updateOp->getParentID());
                    operationNode->setAttribute("newpos", std::to_string(updateOp->getNewPos()));
                } catch (boost::bad_lexical_cast&) {
                }
                updateOp->getElement()->accept(visitor);
                operationNode->addNode(visitor.getResult());

            }

            WhiteboardDeleteOperation::ref deleteOp = std::dynamic_pointer_cast<WhiteboardDeleteOperation>(payload->getOperation());
            if (deleteOp) {
                try {
                    operationNode->setAttribute("type", "delete");
                    operationNode->setAttribute("pos", std::to_string(deleteOp->getPos()));
                    operationNode->setAttribute("id", deleteOp->getID());
                    operationNode->setAttribute("parentid", deleteOp->getParentID());
                    operationNode->setAttribute("elementid", deleteOp->getElementID());
                } catch (boost::bad_lexical_cast&) {
                }
            }
            element.addNode(operationNode);
        }
        element.setAttribute("type", typeToString(payload->getType()));
        return element.serialize();
    }

    std::string WhiteboardSerializer::typeToString(WhiteboardPayload::Type type) const {
        switch (type) {
            case WhiteboardPayload::Data:
                return "data";
            case WhiteboardPayload::SessionRequest:
                return "session-request";
            case WhiteboardPayload::SessionAccept:
                return "session-accept";
            case WhiteboardPayload::SessionTerminate:
                return "session-terminate";
            case WhiteboardPayload::UnknownType:
                SWIFT_LOG(warning) << "Serializing unknown action value." << std::endl;
                return "";
        }
        assert(false);
        return "";
    }
}
