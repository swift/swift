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

#include <Swiften/Parser/PayloadParsers/WhiteboardParser.h>

#include <memory>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <Swiften/Elements/Whiteboard/WhiteboardColor.h>
#include <Swiften/Elements/Whiteboard/WhiteboardDeleteOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardEllipseElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardFreehandPathElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardLineElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardPolygonElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardRectElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardTextElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardUpdateOperation.h>

namespace Swift {
    WhiteboardParser::WhiteboardParser() : actualIsText(false), level_(0) {
    }

    void WhiteboardParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
        if (level_ == 0) {
            getPayloadInternal()->setType(stringToType(attributes.getAttributeValue("type").get_value_or("")));
        } else if (level_ == 1) {
            std::string type = attributes.getAttributeValue("type").get_value_or("");
            if (type == "insert") {
                WhiteboardInsertOperation::ref insertOp = std::make_shared<WhiteboardInsertOperation>();
                operation = insertOp;
            } else if (type == "update") {
                WhiteboardUpdateOperation::ref updateOp = std::make_shared<WhiteboardUpdateOperation>();
                std::string move = attributes.getAttributeValue("newpos").get_value_or("0");
                updateOp->setNewPos(boost::lexical_cast<int>(attributes.getAttributeValue("newpos").get_value_or("0")));
                operation = updateOp;
            } else if (type == "delete") {
                WhiteboardDeleteOperation::ref deleteOp = std::make_shared<WhiteboardDeleteOperation>();
                deleteOp->setElementID(attributes.getAttributeValue("elementid").get_value_or(""));
                operation = deleteOp;
            }
            if (operation) {
                try {
                    operation->setID(attributes.getAttributeValue("id").get_value_or(""));
                    operation->setParentID(attributes.getAttributeValue("parentid").get_value_or(""));
                    operation->setPos(boost::lexical_cast<int>(attributes.getAttributeValue("pos").get_value_or("0")));
                } catch (boost::bad_lexical_cast&) {
                }
            }

        } else if (level_ == 2) {
            if (element == "line") {
                int x1 = 0;
                int y1 = 0;
                int x2 = 0;
                int y2 = 0;
                try {
                    x1 = boost::lexical_cast<int>(attributes.getAttributeValue("x1").get_value_or("0"));
                    y1 = boost::lexical_cast<int>(attributes.getAttributeValue("y1").get_value_or("0"));
                    x2 = boost::lexical_cast<int>(attributes.getAttributeValue("x2").get_value_or("0"));
                    y2 = boost::lexical_cast<int>(attributes.getAttributeValue("y2").get_value_or("0"));
                } catch (boost::bad_lexical_cast&) {
                }
                WhiteboardLineElement::ref whiteboardElement = std::make_shared<WhiteboardLineElement>(x1, y1, x2, y2);

                WhiteboardColor color(attributes.getAttributeValue("stroke").get_value_or("#000000"));
                color.setAlpha(opacityToAlpha(attributes.getAttributeValue("opacity").get_value_or("1")));
                whiteboardElement->setColor(color);

                int penWidth = 1;
                try {
                    penWidth = boost::lexical_cast<int>(attributes.getAttributeValue("stroke-width").get_value_or("1"));
                } catch (boost::bad_lexical_cast&) {
                }
                whiteboardElement->setPenWidth(penWidth);
                whiteboardElement->setID(attributes.getAttributeValue("id").get_value_or(""));
                getPayloadInternal()->setElement(whiteboardElement);
                wbElement = whiteboardElement;
            } else if (element == "path") {
                WhiteboardFreehandPathElement::ref whiteboardElement = std::make_shared<WhiteboardFreehandPathElement>();
                std::string pathData = attributes.getAttributeValue("d").get_value_or("");
                std::vector<std::pair<int, int> > points;
                if (pathData[0] == 'M') {
                    size_t pos = 1;
                    size_t npos;
                    int x, y;
                    if (pathData[pos] == ' ') {
                        pos++;
                    }
                    try {
                        npos = pathData.find(' ', pos);
                        x = boost::lexical_cast<int>(pathData.substr(pos, npos-pos));
                        pos = npos+1;
                        npos = pathData.find('L', pos);
                        y = boost::lexical_cast<int>(pathData.substr(pos, npos-pos));
                        pos = npos+1;
                        if (pathData[pos] == ' ') {
                            pos++;
                        }
                        points.push_back(std::pair<int, int>(x, y));
                        while (pos < pathData.size()) {
                            npos = pathData.find(' ', pos);
                            x = boost::lexical_cast<int>(pathData.substr(pos, npos-pos));
                            pos = npos+1;
                            npos = pathData.find(' ', pos);
                            y = boost::lexical_cast<int>(pathData.substr(pos, npos-pos));
                            pos = npos+1;
                            points.push_back(std::pair<int, int>(x, y));
                        }
                    } catch (boost::bad_lexical_cast&) {
                    }
                }
                whiteboardElement->setPoints(points);

                int penWidth = 1;
                try {
                    penWidth = boost::lexical_cast<int>(attributes.getAttributeValue("stroke-width").get_value_or("1"));
                } catch (boost::bad_lexical_cast&) {
                }
                whiteboardElement->setPenWidth(penWidth);

                WhiteboardColor color(attributes.getAttributeValue("stroke").get_value_or("#000000"));
                color.setAlpha(opacityToAlpha(attributes.getAttributeValue("opacity").get_value_or("1")));
                whiteboardElement->setColor(color);
                whiteboardElement->setID(attributes.getAttributeValue("id").get_value_or(""));
                getPayloadInternal()->setElement(whiteboardElement);
                wbElement = whiteboardElement;
            } else if (element == "rect") {
                int x = 0;
                int y = 0;
                int width = 0;
                int height = 0;
                try {
                    x = boost::lexical_cast<int>(attributes.getAttributeValue("x").get_value_or("0"));
                    y = boost::lexical_cast<int>(attributes.getAttributeValue("y").get_value_or("0"));
                    width = boost::lexical_cast<int>(attributes.getAttributeValue("width").get_value_or("0"));
                    height = boost::lexical_cast<int>(attributes.getAttributeValue("height").get_value_or("0"));
                } catch (boost::bad_lexical_cast&) {
                }

                WhiteboardRectElement::ref whiteboardElement = std::make_shared<WhiteboardRectElement>(x, y, width, height);

                int penWidth = 1;
                try {
                    penWidth = boost::lexical_cast<int>(attributes.getAttributeValue("stroke-width").get_value_or("1"));
                } catch (boost::bad_lexical_cast&) {
                }
                whiteboardElement->setPenWidth(penWidth);

                WhiteboardColor penColor(attributes.getAttributeValue("stroke").get_value_or("#000000"));
                WhiteboardColor brushColor(attributes.getAttributeValue("fill").get_value_or("#000000"));
                penColor.setAlpha(opacityToAlpha(attributes.getAttributeValue("opacity").get_value_or("1")));
                brushColor.setAlpha(opacityToAlpha(attributes.getAttributeValue("fill-opacity").get_value_or("1")));
                whiteboardElement->setPenColor(penColor);
                whiteboardElement->setBrushColor(brushColor);
                whiteboardElement->setID(attributes.getAttributeValue("id").get_value_or(""));
                getPayloadInternal()->setElement(whiteboardElement);
                wbElement = whiteboardElement;
            } else if (element == "polygon") {
                WhiteboardPolygonElement::ref whiteboardElement = std::make_shared<WhiteboardPolygonElement>();

                std::string pointsData = attributes.getAttributeValue("points").get_value_or("");
                std::vector<std::pair<int, int> > points;
                size_t pos = 0;
                size_t npos;
                int x, y;
                try {
                    while (pos < pointsData.size()) {
                        npos = pointsData.find(',', pos);
                        x = boost::lexical_cast<int>(pointsData.substr(pos, npos-pos));
                        pos = npos+1;
                        npos = pointsData.find(' ', pos);
                        y = boost::lexical_cast<int>(pointsData.substr(pos, npos-pos));
                        pos = npos+1;
                        points.push_back(std::pair<int, int>(x, y));
                    }
                } catch (boost::bad_lexical_cast&) {
                }

                whiteboardElement->setPoints(points);

                int penWidth = 1;
                try {
                    penWidth = boost::lexical_cast<int>(attributes.getAttributeValue("stroke-width").get_value_or("1"));
                } catch (boost::bad_lexical_cast&) {
                }
                whiteboardElement->setPenWidth(penWidth);

                WhiteboardColor penColor(attributes.getAttributeValue("stroke").get_value_or("#000000"));
                WhiteboardColor brushColor(attributes.getAttributeValue("fill").get_value_or("#000000"));
                penColor.setAlpha(opacityToAlpha(attributes.getAttributeValue("opacity").get_value_or("1")));
                brushColor.setAlpha(opacityToAlpha(attributes.getAttributeValue("fill-opacity").get_value_or("1")));
                whiteboardElement->setPenColor(penColor);
                whiteboardElement->setBrushColor(brushColor);
                whiteboardElement->setID(attributes.getAttributeValue("id").get_value_or(""));
                getPayloadInternal()->setElement(whiteboardElement);
                wbElement = whiteboardElement;
            } else if (element == "text") {
                int x = 0;
                int y = 0;
                try {
                    x = boost::lexical_cast<int>(attributes.getAttributeValue("x").get_value_or("0"));
                    y = boost::lexical_cast<int>(attributes.getAttributeValue("y").get_value_or("0"));
                } catch (boost::bad_lexical_cast&) {
                }

                WhiteboardTextElement::ref whiteboardElement = std::make_shared<WhiteboardTextElement>(x, y);

                actualIsText = true;
                WhiteboardColor color(attributes.getAttributeValue("fill").get_value_or("#000000"));
                color.setAlpha(opacityToAlpha(attributes.getAttributeValue("opacity").get_value_or("1")));
                whiteboardElement->setColor(color);

                int fontSize = 1;
                try {
                    fontSize = boost::lexical_cast<int>(attributes.getAttributeValue("font-size").get_value_or("12"));
                } catch (boost::bad_lexical_cast&) {
                }
                whiteboardElement->setSize(fontSize);
                whiteboardElement->setID(attributes.getAttributeValue("id").get_value_or(""));
                getPayloadInternal()->setElement(whiteboardElement);
                wbElement = whiteboardElement;
            } else if (element == "ellipse") {
                int cx = 0;
                int cy = 0;
                int rx = 0;
                int ry = 0;
                try {
                    cx = boost::lexical_cast<int>(attributes.getAttributeValue("cx").get_value_or("0"));
                    cy = boost::lexical_cast<int>(attributes.getAttributeValue("cy").get_value_or("0"));
                    rx = boost::lexical_cast<int>(attributes.getAttributeValue("rx").get_value_or("0"));
                    ry = boost::lexical_cast<int>(attributes.getAttributeValue("ry").get_value_or("0"));
                } catch (boost::bad_lexical_cast&) {
                }

                WhiteboardEllipseElement::ref whiteboardElement = std::make_shared<WhiteboardEllipseElement>(cx, cy, rx, ry);

                int penWidth = 1;
                try {
                    penWidth = boost::lexical_cast<int>(attributes.getAttributeValue("stroke-width").get_value_or("1"));
                } catch (boost::bad_lexical_cast&) {
                }
                whiteboardElement->setPenWidth(penWidth);

                WhiteboardColor penColor(attributes.getAttributeValue("stroke").get_value_or("#000000"));
                WhiteboardColor brushColor(attributes.getAttributeValue("fill").get_value_or("#000000"));
                penColor.setAlpha(opacityToAlpha(attributes.getAttributeValue("opacity").get_value_or("1")));
                brushColor.setAlpha(opacityToAlpha(attributes.getAttributeValue("fill-opacity").get_value_or("1")));
                whiteboardElement->setPenColor(penColor);
                whiteboardElement->setBrushColor(brushColor);
                whiteboardElement->setID(attributes.getAttributeValue("id").get_value_or(""));
                getPayloadInternal()->setElement(whiteboardElement);
                wbElement = whiteboardElement;
            }
        }
        ++level_;
    }

    void WhiteboardParser::handleEndElement(const std::string& element, const std::string&) {
        --level_;
        if (level_ == 0) {
            getPayloadInternal()->setData(data_);
        } else if (level_ == 1) {
            WhiteboardInsertOperation::ref insertOp = std::dynamic_pointer_cast<WhiteboardInsertOperation>(operation);
            if (insertOp) {
                insertOp->setElement(wbElement);
            }

            WhiteboardUpdateOperation::ref updateOp = std::dynamic_pointer_cast<WhiteboardUpdateOperation>(operation);
            if (updateOp) {
                updateOp->setElement(wbElement);
            }
            getPayloadInternal()->setOperation(operation);
        } else if (level_ == 2) {
            if (element == "text") {
                actualIsText = false;
            }
        }
    }

    void WhiteboardParser::handleCharacterData(const std::string& data) {
        if (level_ == 3 && actualIsText) {
            WhiteboardTextElement::ref element = std::dynamic_pointer_cast<WhiteboardTextElement>(getPayloadInternal()->getElement());
            element->setText(data);
        }
    }

    WhiteboardPayload::Type WhiteboardParser::stringToType(const std::string& type) const {
        if (type == "data") {
            return WhiteboardPayload::Data;
        } else if (type == "session-request") {
            return WhiteboardPayload::SessionRequest;
        } else if (type == "session-accept") {
            return WhiteboardPayload::SessionAccept;
        } else if (type == "session-terminate") {
            return WhiteboardPayload::SessionTerminate;
        } else {
            return WhiteboardPayload::UnknownType;
        }
    }

    int WhiteboardParser::opacityToAlpha(std::string opacity) const {
        int value = 255;
        if (opacity.find('.') != std::string::npos) {
            opacity = opacity.substr(opacity.find('.')+1, 2);
            try {
                value = boost::lexical_cast<int>(opacity)*255/100;
            } catch (boost::bad_lexical_cast&) {
            }
        }
        return value;
    }
}
