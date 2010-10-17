/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include "Swiften/Elements/Payload.h"
#include "Swiften/Elements/FormField.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/Shared.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	/**
	 * XEP-0004 Data form.
	 * For the relevant Fields, the parsers and serialisers protect the API user against
	 * the strange multi-value instead of newline thing by transforming them.
	 */
	class Form : public Payload, public Shared<Form> {
		public:
			enum Type { FormType, SubmitType, CancelType, ResultType };

		public:
			Form(Type type = FormType) : type_(type) {}

			void addField(boost::shared_ptr<FormField> field) { fields_.push_back(field); }
			const std::vector<boost::shared_ptr<FormField> >& getFields() const { return fields_; }
			void setTitle(const String& title) { title_ = title; }
			const String& getTitle() { return title_; }

			void setInstructions(const String& instructions) { instructions_ = instructions; }
			const String& getInstructions() { return instructions_; }

			Type getType() { return type_; }
			void setType(Type type) { type_ = type; }

			String getFormType() const;

			FormField::ref getField(const String& name) const;

		private:
			std::vector<boost::shared_ptr<FormField> > fields_;
			String title_;
			String instructions_;
			Type type_;
	};
}
