/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/WhiteboardSerializer.h>

#include <iostream>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardUpdateOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardDeleteOperation.h>

namespace Swift {
	void WhiteboardElementSerializingVisitor::visit(WhiteboardLineElement& line) {
		element = boost::make_shared<XMLElement>("line");
		try {
			element->setAttribute("x1", boost::lexical_cast<std::string>(line.x1()));
			element->setAttribute("y1", boost::lexical_cast<std::string>(line.y1()));
			element->setAttribute("x2", boost::lexical_cast<std::string>(line.x2()));
			element->setAttribute("y2", boost::lexical_cast<std::string>(line.y2()));
			element->setAttribute("id", line.getID());
			element->setAttribute("stroke", line.getColor().toHex());
			element->setAttribute("stroke-width", boost::lexical_cast<std::string>(line.getPenWidth()));
			element->setAttribute("opacity", alphaToOpacity(line.getColor().getAlpha()));
		} catch (boost::bad_lexical_cast&) {
		}
	}

	void WhiteboardElementSerializingVisitor::visit(WhiteboardFreehandPathElement& path) {
		element = boost::make_shared<XMLElement>("path");
		element->setAttribute("id", path.getID());
		element->setAttribute("stroke", path.getColor().toHex());
		try {
			element->setAttribute("stroke-width", boost::lexical_cast<std::string>(path.getPenWidth()));
			element->setAttribute("opacity", alphaToOpacity(path.getColor().getAlpha()));
			std::string pathData;
			if (path.getPoints().size() != 0) {
				std::vector<std::pair<int, int> >::const_iterator it = path.getPoints().begin();
				pathData = "M"+boost::lexical_cast<std::string>(it->first)+" "+boost::lexical_cast<std::string>(it->second)+"L";
				for (; it != path.getPoints().end(); ++it) {
					pathData += boost::lexical_cast<std::string>(it->first)+" "+boost::lexical_cast<std::string>(it->second)+" ";
				}
			}
			element->setAttribute("d", pathData);
		} catch (boost::bad_lexical_cast&) {
		}
	}

	void WhiteboardElementSerializingVisitor::visit(WhiteboardRectElement& rect) {
		element = boost::make_shared<XMLElement>("rect");
		try {
			element->setAttribute("x", boost::lexical_cast<std::string>(rect.getX()));
			element->setAttribute("y", boost::lexical_cast<std::string>(rect.getY()));
			element->setAttribute("width", boost::lexical_cast<std::string>(rect.getWidth()));
			element->setAttribute("height", boost::lexical_cast<std::string>(rect.getHeight()));
			element->setAttribute("id", rect.getID());
			element->setAttribute("stroke", rect.getPenColor().toHex());
			element->setAttribute("fill", rect.getBrushColor().toHex());;
			element->setAttribute("stroke-width", boost::lexical_cast<std::string>(rect.getPenWidth()));
			element->setAttribute("opacity", alphaToOpacity(rect.getPenColor().getAlpha()));
			element->setAttribute("fill-opacity", alphaToOpacity(rect.getBrushColor().getAlpha()));
		} catch (boost::bad_lexical_cast&) {
		}
	}

	void WhiteboardElementSerializingVisitor::visit(WhiteboardPolygonElement& polygon) {
		element = boost::make_shared<XMLElement>("polygon");
		try {
			element->setAttribute("id", polygon.getID());
			element->setAttribute("stroke", polygon.getPenColor().toHex());
			element->setAttribute("fill", polygon.getBrushColor().toHex());;
			element->setAttribute("stroke-width", boost::lexical_cast<std::string>(polygon.getPenWidth()));
			element->setAttribute("opacity", alphaToOpacity(polygon.getPenColor().getAlpha()));
			element->setAttribute("fill-opacity", alphaToOpacity(polygon.getBrushColor().getAlpha()));
			std::string points;
			std::vector<std::pair<int, int> >::const_iterator it = polygon.getPoints().begin();
			for (; it != polygon.getPoints().end(); ++it) {
				points += boost::lexical_cast<std::string>(it->first)+","+boost::lexical_cast<std::string>(it->second)+" ";
			}
			element->setAttribute("points", points);
		} catch (boost::bad_lexical_cast&) {
		}
	}

	void WhiteboardElementSerializingVisitor::visit(WhiteboardTextElement& text) {
		element = boost::make_shared<XMLElement>("text");
		try {
			element->setAttribute("x", boost::lexical_cast<std::string>(text.getX()));
			element->setAttribute("y", boost::lexical_cast<std::string>(text.getY()));
			element->setAttribute("font-size", boost::lexical_cast<std::string>(text.getSize()));
			element->setAttribute("id", text.getID());
			element->setAttribute("fill", text.getColor().toHex());
			element->setAttribute("opacity", alphaToOpacity(text.getColor().getAlpha()));
			element->addNode(boost::make_shared<XMLTextNode>(text.getText()));
		} catch (boost::bad_lexical_cast&) {
		}
	}

 	void WhiteboardElementSerializingVisitor::visit(WhiteboardEllipseElement& ellipse) {
		element = boost::make_shared<XMLElement>("ellipse");
		try {
			element->setAttribute("cx", boost::lexical_cast<std::string>(ellipse.getCX()));
			element->setAttribute("cy", boost::lexical_cast<std::string>(ellipse.getCY()));
			element->setAttribute("rx", boost::lexical_cast<std::string>(ellipse.getRX()));
			element->setAttribute("ry", boost::lexical_cast<std::string>(ellipse.getRY()));
			element->setAttribute("id", ellipse.getID());
			element->setAttribute("stroke", ellipse.getPenColor().toHex());
			element->setAttribute("fill", ellipse.getBrushColor().toHex());;
			element->setAttribute("stroke-width", boost::lexical_cast<std::string>(ellipse.getPenWidth()));
			element->setAttribute("opacity", alphaToOpacity(ellipse.getPenColor().getAlpha()));
			element->setAttribute("fill-opacity", alphaToOpacity(ellipse.getBrushColor().getAlpha()));
		} catch (boost::bad_lexical_cast&) {
		}
	}

	std::string WhiteboardElementSerializingVisitor::intToStr(const int t) const {
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	XMLElement::ref WhiteboardElementSerializingVisitor::getResult() const {
		return element;
	}

	std::string WhiteboardElementSerializingVisitor::alphaToOpacity(int alpha) const {
		int opacity = 100*alpha/254;
		if (opacity == 100) {
			return "1";
		} else {
			return "."+boost::lexical_cast<std::string>(opacity);
		}
	}

	std::string WhiteboardSerializer::serializePayload(boost::shared_ptr<WhiteboardPayload> payload) const {
		XMLElement element("wb", "http://swift.im/whiteboard");
		if (payload->getType() == WhiteboardPayload::Data) {
			XMLElement::ref operationNode = boost::make_shared<XMLElement>("operation");
			WhiteboardElementSerializingVisitor visitor;
//			payload->getElement()->accept(visitor);
			WhiteboardInsertOperation::ref insertOp = boost::dynamic_pointer_cast<WhiteboardInsertOperation>(payload->getOperation());
			if (insertOp) {
				try {
					operationNode->setAttribute("type", "insert");
					operationNode->setAttribute("pos", boost::lexical_cast<std::string>(insertOp->getPos()));
					operationNode->setAttribute("id", insertOp->getID());
					operationNode->setAttribute("parentid", insertOp->getParentID());
				} catch (boost::bad_lexical_cast&) {
				}
				insertOp->getElement()->accept(visitor);
				operationNode->addNode(visitor.getResult());
			}
			WhiteboardUpdateOperation::ref updateOp = boost::dynamic_pointer_cast<WhiteboardUpdateOperation>(payload->getOperation());
			if (updateOp) {
				try {
					operationNode->setAttribute("type", "update");
					operationNode->setAttribute("pos", boost::lexical_cast<std::string>(updateOp->getPos()));
					operationNode->setAttribute("id", updateOp->getID());
					operationNode->setAttribute("parentid", updateOp->getParentID());
					operationNode->setAttribute("newpos", boost::lexical_cast<std::string>(updateOp->getNewPos()));
				} catch (boost::bad_lexical_cast&) {
				}
				updateOp->getElement()->accept(visitor);
				operationNode->addNode(visitor.getResult());

			}

			WhiteboardDeleteOperation::ref deleteOp = boost::dynamic_pointer_cast<WhiteboardDeleteOperation>(payload->getOperation());
			if (deleteOp) {
				try {
					operationNode->setAttribute("type", "delete");
					operationNode->setAttribute("pos", boost::lexical_cast<std::string>(deleteOp->getPos()));
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
				std::cerr << "Warning: Serializing unknown action value." << std::endl;
				return "";
		}
		assert(false);
		return "";
	}
}
