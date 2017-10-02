/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Whiteboard/WhiteboardElementVisitor.h>
#include <Swiften/Elements/Whiteboard/WhiteboardEllipseElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardFreehandPathElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardLineElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardPolygonElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardRectElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardTextElement.h>
#include <Swiften/Elements/WhiteboardPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API WhiteboardElementSerializingVisitor : public WhiteboardElementVisitor {
    public:
        void visit(WhiteboardLineElement& line);
        void visit(WhiteboardFreehandPathElement& path);
        void visit(WhiteboardRectElement& rect);
        void visit(WhiteboardPolygonElement& polygon);
        void visit(WhiteboardTextElement& text);
        void visit(WhiteboardEllipseElement& ellipse);
        XMLElement::ref getResult() const;

    private:
        std::string alphaToOpacity(int alpha) const;

        XMLElement::ref element;
    };

    class SWIFTEN_API WhiteboardSerializer : public GenericPayloadSerializer<WhiteboardPayload> {
    public:
        std::string serializePayload(std::shared_ptr<WhiteboardPayload> payload) const;

    private:
        std::string typeToString(WhiteboardPayload::Type type) const;
    };
}
