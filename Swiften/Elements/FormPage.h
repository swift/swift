/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/FormField.h>
#include <Swiften/Elements/FormReportedRef.h>
#include <Swiften/Elements/FormSection.h>
#include <Swiften/Elements/FormText.h>

namespace Swift {

	class SWIFTEN_API FormPage {
		public:
			typedef boost::shared_ptr<FormPage> page;
			FormPage ();
			~FormPage();
			void setLabel(const std::string& label);
			const std::string& getLabel() const;
			void addChildSection(boost::shared_ptr<FormSection> section);
			const std::vector<boost::shared_ptr<FormSection> >& getChildSections() const;
			void addTextElement(boost::shared_ptr<FormText> textElement);
			const std::vector<boost::shared_ptr<FormText> >& getTextElements() const;
			void addReportedRef(boost::shared_ptr<FormReportedRef> reportedRef);
			const std::vector<boost::shared_ptr<FormReportedRef> >& getReportedRefs() const;
			void addField(boost::shared_ptr<FormField> field);
			const std::vector<boost::shared_ptr<FormField> >& getFields() const;
			void addFieldRef(std::string ref);
			const std::vector<std::string> getFieldRefs() const;

		private:
			std::string label_;
			std::vector<boost::shared_ptr<FormText> > textElements_;
			std::vector<boost::shared_ptr<FormSection> > childSections_;
			std::vector<boost::shared_ptr<FormReportedRef> > reportedRefs_;
			std::vector<boost::shared_ptr<FormField> > fields_;
			std::vector<std::string> fieldRefs_;
	};
}
