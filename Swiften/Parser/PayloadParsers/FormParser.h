/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API FormParser : public GenericPayloadParser<Form> {
        public:
            FormParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

            enum Level {
                TopLevel = 0,
                PayloadLevel = 1,
                FieldLevel = 2
            };
            int level_;
            std::string currentText_;
            std::string currentOptionLabel_;
            std::string currentOptionValue_;
            std::string currentFieldRef_;
            bool parsingItem_;
            bool parsingReported_;
            bool parsingOption_;
            bool hasReportedRef_;
            FormField::ref currentField_;
            std::vector<FormField::ref> currentFields_;
            FormText::text currentTextElement_;
            FormReportedRef::ref currentReportedRef_;
            FormPage::page currentPage_;
            FormSection::section currentSection_;
            std::vector<std::shared_ptr<FormPage> > currentPages_;
            std::vector<std::shared_ptr<FormSection> > sectionStack_;
            std::vector<std::shared_ptr<FormSection> > currentSections_;
    };
}
