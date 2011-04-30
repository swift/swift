/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Elements/Subject.h>

namespace Swift {
	class SubjectSerializer : public GenericPayloadSerializer<Subject> {
		public:
			SubjectSerializer() : GenericPayloadSerializer<Subject>() {}

			virtual std::string serializePayload(boost::shared_ptr<Subject> subject)  const {
				XMLTextNode textNode(subject->getText());
				return "<subject>" + textNode.serialize() + "</subject>";
			}
	};
}
