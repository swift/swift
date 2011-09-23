/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#pragma once

#include <string>
#include <vector>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Parser/AttributeMap.h>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {
class ParserElement {
	public:
		typedef boost::shared_ptr<ParserElement> ref;
		ParserElement(const std::string& name, const std::string& xmlns, const AttributeMap& attributes);
		virtual ~ParserElement();
		virtual operator bool();
		ParserElement::ref addChild(const std::string& name, const std::string& xmlns, const AttributeMap& attributes);
		void appendCharacterData(const std::string& data);
		const std::string& getText() const {return text_;};
		const std::string& getName() const {return name_;};
		const std::string& getNamespace() const {return xmlns_;};
		std::vector<ParserElement::ref> getChildren(const std::string& name, const std::string& xmlns) const;
		ParserElement::ref getChild(const std::string& name, const std::string& xmlns) const;
		const AttributeMap& getAttributes() const {return attributes_;}
	private:
		std::vector<ParserElement::ref> children_;
		std::string name_;
		std::string xmlns_;
		AttributeMap attributes_;
		std::string text_;

};

}
