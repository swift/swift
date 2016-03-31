/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

namespace Swift {
    class WhiteboardLineElement;
    class WhiteboardFreehandPathElement;
    class WhiteboardRectElement;
    class WhiteboardPolygonElement;
    class WhiteboardTextElement;
    class WhiteboardEllipseElement;

    class WhiteboardElementVisitor {
    public:
        virtual ~WhiteboardElementVisitor() {}
        virtual void visit(WhiteboardLineElement& /*element*/) = 0;
        virtual void visit(WhiteboardFreehandPathElement& /*element*/) = 0;
        virtual void visit(WhiteboardRectElement& /*element*/) = 0;
        virtual void visit(WhiteboardPolygonElement& /*element*/) = 0;
        virtual void visit(WhiteboardTextElement& /*element*/) = 0;
        virtual void visit(WhiteboardEllipseElement& /*element*/) = 0;
    };
}
