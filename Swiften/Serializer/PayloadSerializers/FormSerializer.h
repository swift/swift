/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/FormField.h>
#include <Swiften/Elements/FormPage.h>
#include <Swiften/Elements/FormSection.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class SWIFTEN_API FormSerializer : public GenericPayloadSerializer<Form> {
		public:
			FormSerializer();
			virtual std::string serializePayload(boost::shared_ptr<Form>)  const;

		private:
			boost::shared_ptr<XMLElement> textToXML(boost::shared_ptr<FormText> textElement) const;
			boost::shared_ptr<XMLElement> fieldRefToXML(const std::string& ref) const;
			boost::shared_ptr<XMLElement> reportedRefToXML(boost::shared_ptr<FormReportedRef> reportedRef) const;
			boost::shared_ptr<XMLElement> pageToXML(boost::shared_ptr<FormPage> page) const;
			boost::shared_ptr<XMLElement> sectionToXML(boost::shared_ptr<FormSection> section) const;
			boost::shared_ptr<XMLElement> fieldToXML(boost::shared_ptr<FormField> field, bool withTypeAttribute) const;
			void multiLineify(const std::string& text, const std::string& elementName, boost::shared_ptr<XMLElement> parent) const;
			mutable std::vector<boost::shared_ptr<FormField> > fields_;
	};
}
