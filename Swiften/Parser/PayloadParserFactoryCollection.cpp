#include <boost/bind.hpp>
#include <algorithm>

#include "Swiften/Parser/PayloadParserFactoryCollection.h"
#include "Swiften/Parser/PayloadParserFactory.h"

namespace Swift {

PayloadParserFactoryCollection::PayloadParserFactoryCollection() {
}

void PayloadParserFactoryCollection::addFactory(PayloadParserFactory* factory) {
	factories_.push_back(factory);
}

void PayloadParserFactoryCollection::removeFactory(PayloadParserFactory* factory) {
	factories_.erase(remove(factories_.begin(), factories_.end(), factory), factories_.end());
}

PayloadParserFactory* PayloadParserFactoryCollection::getPayloadParserFactory(const String& element, const String& ns, const AttributeMap& attributes) {
	std::vector<PayloadParserFactory*>::const_iterator i = std::find_if(
			factories_.begin(), factories_.end(), 
			boost::bind(&PayloadParserFactory::canParse, _1, element, ns, attributes));
	return (i != factories_.end() ? *i : NULL);
}

}
