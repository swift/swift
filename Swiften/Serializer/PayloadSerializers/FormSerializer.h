/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/FormField.h>
#include <Swiften/Elements/FormPage.h>
#include <Swiften/Elements/FormSection.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API FormSerializer : public GenericPayloadSerializer<Form> {
        public:
            FormSerializer();
            virtual std::string serializePayload(std::shared_ptr<Form>)  const;

        private:
            std::shared_ptr<XMLElement> textToXML(std::shared_ptr<FormText> textElement) const;
            std::shared_ptr<XMLElement> fieldRefToXML(const std::string& ref) const;
            std::shared_ptr<XMLElement> reportedRefToXML(std::shared_ptr<FormReportedRef> reportedRef) const;
            std::shared_ptr<XMLElement> pageToXML(std::shared_ptr<FormPage> page) const;
            std::shared_ptr<XMLElement> sectionToXML(std::shared_ptr<FormSection> section) const;
            std::shared_ptr<XMLElement> fieldToXML(std::shared_ptr<FormField> field, bool withTypeAttribute) const;
            void multiLineify(const std::string& text, const std::string& elementName, std::shared_ptr<XMLElement> parent) const;
            mutable std::vector<std::shared_ptr<FormField> > fields_;
    };
}
