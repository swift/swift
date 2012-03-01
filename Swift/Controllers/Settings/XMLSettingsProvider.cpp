/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Settings/XMLSettingsProvider.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Parser/XMLParser.h>
#include <Swiften/Base/Log.h>

namespace Swift {

XMLSettingsProvider::XMLSettingsProvider(const std::string& xmlConfig) : level_(0) {
	if (!xmlConfig.empty()) {
		PlatformXMLParserFactory factory;
		XMLParser* parser = factory.createXMLParser(this);
		if (parser->parse(xmlConfig)) {
			SWIFT_LOG(debug) << "Found and parsed system config" << std::endl;
		}
		else {
			SWIFT_LOG(debug) << "Found invalid system config" << std::endl;
		}
		delete parser;
	}
	else {
		SWIFT_LOG(debug) << "No system config found" << std::endl;
	}
}

XMLSettingsProvider::~XMLSettingsProvider() {

}

bool XMLSettingsProvider::hasSetting(const std::string& key) {
	return (values_.find(key) != values_.end());
}


std::string XMLSettingsProvider::getSetting(const Setting<std::string>& setting) {
	if (values_.find(setting.getKey()) != values_.end()) {
		std::string value = values_[setting.getKey()];
		return value;
	}
	return setting.getDefaultValue();
}

void XMLSettingsProvider::storeSetting(const Setting<std::string>& /*settingPath*/, const std::string& /*settingValue*/) {
	assert(false);
}

bool XMLSettingsProvider::getSetting(const Setting<bool>& setting) {
	if (values_.find(setting.getKey()) != values_.end()) {
		std::string value = values_[setting.getKey()];
		return boost::iequals(value, "true") || value == "1";
	}
	return setting.getDefaultValue();
}

void XMLSettingsProvider::storeSetting(const Setting<bool>& /*settingPath*/, const bool& /*settingValue*/) {
	assert(false);
}

int XMLSettingsProvider::getSetting(const Setting<int>& setting) {
	if (values_.find(setting.getKey()) != values_.end()) {
		std::string value = values_[setting.getKey()];
		try {
			return value.empty() ? setting.getDefaultValue() : boost::lexical_cast<int>(value);;
		}
		catch(boost::bad_lexical_cast &) {}
	}
	return setting.getDefaultValue();
}

void XMLSettingsProvider::storeSetting(const Setting<int>& /*settingPath*/, const int& /*settingValue*/) {
	assert(false);
}

std::vector<std::string> XMLSettingsProvider::getAvailableProfiles() {
	assert(false);
	return std::vector<std::string>();
}

void XMLSettingsProvider::createProfile(const std::string& /*profile*/) {
	assert(false);
}

void XMLSettingsProvider::removeProfile(const std::string& /*profile*/) {
	assert(false);
}

bool XMLSettingsProvider::getIsSettingFinal(const std::string& settingPath) {
	return finals_.count(settingPath);
}

void XMLSettingsProvider::handleStartElement(const std::string& element, const std::string& /*ns*/, const AttributeMap& attributes) {
	level_++;
	if (level_ == SettingLevel) {
		if (attributes.getBoolAttribute("final", false)) {
			finals_.insert(element);
		}
		currentElement_ = element;
		currentText_ = "";
	}
}

void XMLSettingsProvider::handleEndElement(const std::string& /*element*/, const std::string& /*ns*/) {
	if (level_ == SettingLevel) {
		values_[currentElement_] = currentText_;
		SWIFT_LOG(debug) << "Setting value of " << currentElement_ << " to " << currentText_ << std::endl;
	}
	level_--;
}

void XMLSettingsProvider::handleCharacterData(const std::string& data) {
	if (level_ >= SettingLevel) {
		currentText_ += data;
	}
}


}




