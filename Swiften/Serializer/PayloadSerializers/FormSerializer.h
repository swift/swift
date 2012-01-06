/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/FormField.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class FormSerializer : public GenericPayloadSerializer<Form> {
		public:
			FormSerializer();

			virtual std::string serializePayload(boost::shared_ptr<Form>)  const;

		private:
			boost::shared_ptr<XMLElement> fieldToXML(boost::shared_ptr<FormField> field, bool withTypeAttribute) const;
			void multiLineify(const std::string& text, const std::string& elementName, boost::shared_ptr<XMLElement> parent) const;
	};
}


