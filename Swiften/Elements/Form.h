/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/FormField.h>
#include <Swiften/Elements/FormPage.h>
#include <Swiften/Elements/FormSection.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	/**
	 * XEP-0004 Data form.
	 * For the relevant Fields, the parsers and serialisers protect the API user against
	 * the strange multi-value instead of newline thing by transforming them.
	 */
	class SWIFTEN_API Form : public Payload {
		public:
			typedef boost::shared_ptr<Form> ref;
			typedef std::vector<FormField::ref> FormItem;

			enum Type { FormType, SubmitType, CancelType, ResultType };

		public:
			Form(Type type = FormType) : type_(type) {}

			void addPage(boost::shared_ptr<FormPage> page) {
				assert(page);
				pages_.push_back(page);
			}

			const std::vector<boost::shared_ptr<FormPage> >& getPages() const {
				return pages_;
			}

			void addField(boost::shared_ptr<FormField> field) {
				assert(field);
				fields_.push_back(field);
			}

			const std::vector<boost::shared_ptr<FormField> >& getFields() const {
				return fields_;
			}

			void clearFields() {
				fields_.clear();
			}

			void addTextElement(boost::shared_ptr<FormText> text) {
				assert(text);
				textElements_.push_back(text);
			}

			const std::vector<boost::shared_ptr<FormText> >& getTextElements() const {
				return textElements_;
			}

			void addReportedRef(boost::shared_ptr<FormReportedRef> reportedRef) {
				assert(reportedRef);
				reportedRefs_.push_back(reportedRef);
			}

			const std::vector<boost::shared_ptr<FormReportedRef> >& getReportedRefs() const {
				return reportedRefs_;
			}

			void setTitle(const std::string& title) {
				title_ = title;
			}

			const std::string& getTitle() const {
				return title_;
			}

			void setInstructions(const std::string& instructions) {
				instructions_ = instructions;
			}

			const std::string& getInstructions() const {
				return instructions_;
			}

			Type getType() const {
				return type_;
			}

			void setType(Type type) {
				type_ = type;
			}

			std::string getFormType() const;
			FormField::ref getField(const std::string& name) const;
			void addItem(const FormItem& item);
			const std::vector<FormItem>& getItems() const;
			void clearItems() { items_.clear(); }

			void clearEmptyTextFields();

			void addReportedField(FormField::ref field);
			const std::vector<FormField::ref> & getReportedFields() const;
			void clearReportedFields() { reportedFields_.clear(); }

		private:
			std::vector<boost::shared_ptr<FormReportedRef> >reportedRefs_;
			std::vector<boost::shared_ptr<FormText> > textElements_;
			std::vector<boost::shared_ptr<FormPage> > pages_;
			std::vector<boost::shared_ptr<FormField> > fields_;
			std::vector<boost::shared_ptr<FormField> > reportedFields_;
			std::vector<FormItem> items_;
			boost::shared_ptr<FormReportedRef> reportedRef_;
			std::string title_;
			std::string instructions_;
			Type type_;
	};
}
